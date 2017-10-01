#include "Estimator.h"
using namespace arma;


namespace Nf
{
  /// \brief		Constructor: Initializes filter	
  Estimator::Estimator(void)
  {
    Init =  false;
    OBS.clear();
    INS.clear();
    YAW.clear();

    slen = sizeof(si_other) ;
     
    //Initialise winsock
    printf("\nInitialising Winsock...");
    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
    {
        printf("Failed. Error Code : %d",WSAGetLastError());
        exit(EXIT_FAILURE);
    }
    printf("Initialised.\n");
     
    //Create a socket
    if((s = socket(AF_INET , SOCK_DGRAM , 0 )) == INVALID_SOCKET)
    {
        printf("Could not create socket : %d" , WSAGetLastError());
    }
    printf("Socket created.\n");
     
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( PORT );

    si_other.sin_family = AF_INET;
    si_other.sin_addr.s_addr = INADDR_ANY;
    si_other.sin_port = htons( PORT+1);


    //Bind
    if( bind(s ,(struct sockaddr *)&server , sizeof(server)) == SOCKET_ERROR)
    {
        printf("Bind failed with error code : %d" , WSAGetLastError());
        exit(EXIT_FAILURE);
    }
    puts("Bind done");
 
  }

  /// \brief		Destructor
  Estimator::~Estimator(void)
  {
    closesocket(s);
    WSACleanup();
  }

  void Estimator::setEstimator(float ins,float appoff,float yaw)
  {
    Init =  true;

    ROT_base = Matrix33d::I();
    off = Vec3d(0,0,appoff+ins);
    prev_l = ins;
    prev_th = yaw;

    tip_mm_t = ROT_base*off;
    ROT_t = ROT_base;
    rho = 85; //mm 

    TIP_t.clear();
    TIP_t.push_back(Matrix44d::FromOrientationAndTranslation(ROT_t, tip_mm_t));
    e_roll = e_pitch = e_yaw = 0;
  }
  void Estimator::resetEstimator(void)
  {
    Init =  false;
  }
  bool Estimator::isInitialized(void)
  {
    return Init;
  }
  Matrix44d Estimator::getCurrentEstimate()
  {
    return TIP_t.back();
  }
  void Estimator::addPOINT(Vec3d p)
  {
    OBS.push_back(p);
  }

  
  Matrix33d Estimator::rotx(double roll)
  {
     double ct;
     double st;

     ct=cos(roll);
     st=sin(roll);

     return Matrix33d(1,0,0,0,ct,-st,0,st,ct);
  }
  
  Matrix33d Estimator::roty(double pitch)
  {
     double ct;
     double st;

     ct=cos(pitch);
     st=sin(pitch);

     return Matrix33d(ct,0,st,0,1,0,-st,0,ct);
  }

  Matrix33d Estimator::rotz(double yaw)
  {
     double ct;
     double st;

     ct=cos(yaw);
     st=sin(yaw);

     return Matrix33d(ct,-st,0,st,ct,0,0,0,1);
  }


  void Estimator::updateInput(double m_l, double m_th)
  {
    double diff_l,diff_th;
    Vec3d app_d;

    diff_l = m_l - prev_l;
    diff_th = m_th - prev_th;
    
    // Gradi?
    if ((abs(diff_l)>0.0001)||(abs(diff_th)>0.0001))
    {
       
       INS.push_back(diff_l);
       YAW.push_back(diff_th);

       app_d = Vec3d(0,rho*(1-cos(diff_l/rho)),rho*sin(diff_l/rho));

       tip_mm_t = tip_mm_t + ROT_t * rotz(diff_th*(PI/180)) * app_d;
       ROT_t = ROT_t * rotz(diff_th*(PI/180)) * rotx(-(diff_l/rho));

       TIP_t.push_back(Matrix44d::FromOrientationAndTranslation(ROT_t, tip_mm_t));

       prev_l = m_l;
       prev_th = m_th;
    }

  }

  void Estimator::saveDataOpt()
  {
    mat dyaw = zeros(1,YAW.size()); 
    mat dins = zeros(1,INS.size());
    mat point = zeros(3,OBS.size());
    mat R0 = zeros(3,3);
    mat offset = zeros(3,1);
    mat mrho = zeros(1,1);
    mrho(0,0)=rho;
    

    for (int i=0;i<YAW.size();i++)
    {
       dyaw(0,i) = YAW[i];
       dins(0,i) = INS[i];
    }

    Vec3d app;
    for (int i=0;i<OBS.size();i++)
    {
       app = OBS[i];
       point(0,i) = app.x;
       point(1,i) = app.y;
       point(2,i) = app.z;
    }
    
    offset(0,0) = off.x;
    offset(1,0) = off.y;
    offset(2,0) = off.z;


    for (int i=0;i<3;i++)
    {
        for (int j=0;j<3;j++)
        {
            R0(i,j)=ROT_base.m_data[i][j];
        }
    }
    

    char basePath[150];
    char path[150];
    strcpy (basePath , "./"); 

    sprintf(path, "%sYAW.dat", basePath);
    dyaw.save(path, raw_ascii);
    sprintf(path, "%sINS.dat", basePath);
    dins.save(path, raw_ascii);
    sprintf(path, "%sPOINT.dat", basePath);
    point.save(path, raw_ascii);
    sprintf(path, "%sR0.dat", basePath);
    R0.save(path, raw_ascii);
    sprintf(path, "%sOFF.dat", basePath);
    offset.save(path, raw_ascii);
    sprintf(path, "%sRHO.dat", basePath);
    mrho.save(path, raw_ascii);

    
    //now reply the client with the same data
    if (sendto(s, buf, 1, 0, (struct sockaddr*) &si_other, slen) == SOCKET_ERROR)
    {
        int a =WSAGetLastError();
        printf("sendto() failed with error code : %d" , WSAGetLastError());
        //exit(EXIT_FAILURE);
    }
  }

  void Estimator::WaitAndCorrect()
  {
    
    fflush(stdout);
    //clear the buffer by filling null, it might have previously received data
    memset(buf,'\0', BUFLEN);
         
    //try to receive some data, this is a blocking call
    if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == SOCKET_ERROR)
    {
        printf("recvfrom() failed with error code : %d" , WSAGetLastError());
        exit(EXIT_FAILURE);
    }
    
    mat res = zeros(4,1);
    char path[150];
    strcpy (path , "./X.dat"); 
    res.save(path, raw_ascii);
    
    ROT_base = ROT_base * rotx(res(0,0)) *  roty(res(1,0)) *  rotz(res(2,0));
    tip_mm_t = ROT_base*off;
    ROT_t = ROT_base;
    rho = rho + res(3,0); //mm
    TIP_t.clear();
    TIP_t.push_back(Matrix44d::FromOrientationAndTranslation(ROT_t, tip_mm_t));
    
    double diff_l,diff_th;
    Vec3d app_d;    
    for (int i=0;i<INS.size();i++)
    {
      diff_l = INS[i];
      diff_th = YAW[i];
      app_d = Vec3d(0,rho*(1-cos(diff_l/rho)),rho*sin(diff_l/rho));
      tip_mm_t = tip_mm_t + ROT_t * rotz(diff_th*(PI/180)) * app_d;
      ROT_t = ROT_t * rotz(diff_th*(PI/180)) * rotx(-(diff_l/rho));
      TIP_t.push_back(Matrix44d::FromOrientationAndTranslation(ROT_t, tip_mm_t));
    }

  }
}
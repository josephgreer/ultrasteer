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
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons( PORT );

    si_other.sin_family = AF_INET;
    si_other.sin_addr.s_addr = inet_addr("127.0.0.1");
    si_other.sin_port = htons(PORT+1);


    //Bind
    if( bind(s ,(struct sockaddr *)&server , sizeof(server)) == SOCKET_ERROR)
    {
        printf("Bind failed with error code : %d" , WSAGetLastError());
        exit(EXIT_FAILURE);
    }
    puts("Bind done");
    
    u_long iMode = 0;
    ioctlsocket(s, FIONBIO, &iMode);
 
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

    rho_n = 85; //mm 
    rho_a = 40; //mm

    firstAdd = true;

    TIP_t.clear();
    TIP_t.push_back(Matrix44d::FromOrientationAndTranslation(ROT_base, ROT_base*off));
    
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
     Vec3d app_tip = TIP_t.back().GetPosition();     
     Vec3d v_z= TIP_t.back().GetOrientation().Col(2);
     Matrix33d appROT = TIP_t.back().GetOrientation();
     if (!ART.empty())
       if (ART.back())
       {
         appROT = TIP_t.back().GetOrientation()*roty(50*(PI/180));
         v_z= appROT.Col(2);
       }
    app_tip = app_tip  + (v_z)*TIP_LENGTH;
    return Matrix44d::FromOrientationAndTranslation(appROT, app_tip);
  }
  void Estimator::addPOINT(Vec3d p,Vec3d v)
  {
    OBS.push_back(p);
    VER.push_back(v);
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


  void Estimator::updateInput(double m_l, double m_th,unsigned int a)
  {
    double diff_l,diff_th;
    Vec3d app_d,tip_mm_t;
    Matrix33d ROT_t;
    diff_l = m_l - prev_l;
    diff_th = m_th - prev_th;
    
    double rho;
    if ((abs(diff_l)>0.0001)||(abs(diff_th)>0.0001))
    {
       if (diff_l>0)
       {
         INS.push_back(diff_l);
         YAW.push_back(diff_th);
         L.push_back(m_l);
         THETA.push_back(m_th);
         ART.push_back(a);

         tip_mm_t = TIP_t.back().GetPosition();
         ROT_t = TIP_t.back().GetOrientation();

         
         if (a)
            rho = rho_a;
         else
            rho = rho_n;
        
         app_d = Vec3d(0,rho*(1-cos(diff_l/rho)),rho*sin(diff_l/rho));
       
         tip_mm_t = tip_mm_t + ROT_t * rotz(diff_th*(PI/180)) * app_d;
         ROT_t = ROT_t * rotz(diff_th*(PI/180)) * rotx(-(diff_l/rho));

         TIP_t.push_back(Matrix44d::FromOrientationAndTranslation(ROT_t, tip_mm_t));

       }
       else
       {
         while(1)
         {
           if (L.empty())
             break;
           
           if ((m_l<L.back()))
           {  
              L.pop_back();
              INS.pop_back();
              YAW.pop_back();
              TIP_t.pop_back();
              THETA.pop_back();
              ART.pop_back();
                
              for (int i=0;i<IND.size();i++)
              {
                  if (L.size()<IND[i])
                  {
                    OBS.erase(OBS.begin()+i);
                    VER.erase(VER.begin()+i);
                    IND.erase(IND.begin()+i);
                  }

              }
           }
           else
           {  // rotation along z di 
             Matrix33d aMAT = TIP_t.back().GetOrientation()*rotz(-(THETA.back()-m_th)*(PI/180));
             TIP_t.back() = Matrix44d::FromOrientationAndTranslation(aMAT, TIP_t.back().GetPosition());
             break;
           }
           // Add point da cancellare
         }
         
      }

    prev_l = m_l;
    prev_th = m_th;

    }
    
  }

  void Estimator::saveDataOpt()
  {
    mat dyaw = zeros(1,YAW.size()); 
    mat dins = zeros(1,INS.size());
    mat point = zeros(3,OBS.size());
    mat ver = zeros(3,VER.size());
    mat R0 = zeros(3,3);
    mat offset = zeros(3,1);
    mat dart = zeros(1,ART.size());

    mat mrho_n = zeros(1,1);
    mat mrho_a = zeros(1,1);

    mrho_n(0,0)=rho_n;
    mrho_a(0,0)=rho_a;
    
    for (int i=0;i<YAW.size();i++)
    {
       dyaw(0,i) = YAW[i];
       dins(0,i) = INS[i];
       dart(0,i) = ART[i];
    }

    Vec3d app;
    for (int i=0;i<OBS.size();i++)
    {
       app = OBS[i];
       point(0,i) = app.x;
       point(1,i) = app.y;
       point(2,i) = app.z;
    }

    for (int i=0;i<VER.size();i++)
    {
       app = VER[i];
       ver(0,i) = app.x;
       ver(1,i) = app.y;
       ver(2,i) = app.z;
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
    sprintf(path, "%VER.dat", basePath);
    ver.save(path, raw_ascii);
    sprintf(path, "%sR0.dat", basePath);
    R0.save(path, raw_ascii);
    sprintf(path, "%sOFF.dat", basePath);
    offset.save(path, raw_ascii);
    sprintf(path, "%sRHO_N.dat", basePath);
    mrho_n.save(path, raw_ascii);
    sprintf(path, "%sRHO_A.dat", basePath);
    mrho_a.save(path, raw_ascii);
    sprintf(path, "%sART.dat", basePath);
    dart.save(path, raw_ascii);

    
    //now reply the client with the same data
    if (sendto(s, buf, 1, 0, (struct sockaddr*) &si_other, slen) == SOCKET_ERROR)
    {
        int a =WSAGetLastError();
        printf("sendto() failed with error code : %d" , WSAGetLastError());
        //exit(EXIT_FAILURE);
    }

  }

  bool Estimator::WaitAndCorrect()
  {
    
    fflush(stdout);
    //clear the buffer by filling null, it might have previously received data
    memset(buf,'\0', BUFLEN);
    
    fd_set fds ;
    int n ;
    struct timeval tv ;

    // Set up the file descriptor set.
    FD_ZERO(&fds) ;
    FD_SET(s, &fds) ;

    // Set up the struct timeval for the timeout.
    tv.tv_sec = 0;
    tv.tv_usec = 5000 ;

    // Wait until timeout or data received.
    n = select ( s, &fds, NULL, NULL, &tv ) ;
    if ( n == 0)
    {
      return false ;
    }
    else if( n == -1 )
    {
      return false;   
    }
    //try to receive some data, this is a non-blocking call
    recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen);
    if (recv_len<=0)
      return false;
   
    
    mat res = zeros(5,1);
    //mat PointInd = zeros(OBS.size(),1);
    mat PointInd,POINT_NEW,VER_NEW;
    char basePath[150];
    char path[150];

    strcpy (basePath , "./"); 
    sprintf(path, "%sX.dat", basePath);
    res.load(path, raw_ascii);

    strcpy (basePath , "./"); 
    sprintf(path, "%sXa.dat", basePath);
    PointInd.load(path, raw_ascii);

    strcpy (basePath , "./"); 
    sprintf(path, "%sPOINTNEW.dat", basePath);
    POINT_NEW.load(path, raw_ascii);

    strcpy (basePath , "./"); 
    sprintf(path, "%sVERNEW.dat", basePath);
    VER_NEW.load(path, raw_ascii);

    IND.clear();
    OBS.clear();
    VER.clear();
    Vec3d app_v;
    for (int i=0;i<PointInd.size();i++)
    {
       IND.push_back(PointInd(i,0));
       app_v.x = POINT_NEW(0,i);
       app_v.y = POINT_NEW(1,i);
       app_v.z = POINT_NEW(2,i);
       OBS.push_back(app_v);

       app_v.x = VER_NEW(0,i);
       app_v.y = VER_NEW(1,i);
       app_v.z = VER_NEW(2,i);
       VER.push_back(app_v);
    }

    Vec3d tip_mm_t;
    Matrix33d ROT_t;
    
    ROT_base = ROT_base * rotx(res(0,0)) *  roty(res(1,0)) *  rotz(res(2,0));
    tip_mm_t = ROT_base*off;
    ROT_t = ROT_base;
    rho_n = rho_n + res(3,0); //mm
    rho_a = rho_a + res(4,0);

    TIP_t.clear();
    TIP_t.push_back(Matrix44d::FromOrientationAndTranslation(ROT_t, tip_mm_t));
    
    double diff_l,diff_th;
    Vec3d app_d; 
    double rho;
    for (int i=0;i<INS.size();i++)
    {
      diff_l = INS[i];
      diff_th = YAW[i];

      if (ART[i])
        rho = rho_a;
      else
        rho = rho_n;

      app_d = Vec3d(0,rho*(1-cos(diff_l/rho)),rho*sin(diff_l/rho));
      tip_mm_t = tip_mm_t + ROT_t * rotz(diff_th*(PI/180)) * app_d;
      ROT_t = ROT_t * rotz(diff_th*(PI/180)) * rotx(-(diff_l/rho));
      TIP_t.push_back(Matrix44d::FromOrientationAndTranslation(ROT_t, tip_mm_t));
    }
    return true;
  }

  void Estimator::addTIP()
  {
    if(!firstAdd)
      return;
    firstAdd = false;

    double disp = 0.01;
    Matrix44d Wrist = TIP_t.back();
   
    Vec3d P= Wrist.GetPosition();
    Vec3d app_tip;
    double appL = disp;
    Matrix33d appROT;

    Vec3d v_z= Wrist.GetOrientation().Col(2);
    if (!ART.empty())
      if (ART.back())
      {
        appROT = TIP_t.back().GetOrientation()*roty(50*(PI/180));
        v_z= appROT.Col(2);
      }

    while(appL < TIP_LENGTH)
    {
      app_tip = P + v_z*appL;
      TIP_t.push_back(Matrix44d::FromOrientationAndTranslation(appROT, app_tip));
      appL = appL + disp; 
    }

  }
  void Estimator::resetaddTIP()
  {
    firstAdd = true;
  }
}


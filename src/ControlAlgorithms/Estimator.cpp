#include "Estimator.h"
using namespace arma;
#define INTR_LENGTH 15

#define FAULHABER_ONE_ROTATION1	400						//# of encoder ticks in one full rotation (without factoring in the gear ratio) 
#define SPUR_GEAR_RATIO1			(1539.0/65.0*4.0/0.5)	//Gear ratio for the maxon motor gearhead + pinion gears


namespace Nf
{
  /// \brief		Constructor: Initializes filter	
  Estimator::Estimator(void)
  {
    Init =  false;
    OBS.clear();
    L.clear();
    ART.clear();
    IND.clear();
    THETA.clear();

    ghMutex = CreateMutex( 
        NULL,              // default security attributes
        FALSE,             // initially not owned
        NULL);             // unnamed mutex
    
    StartArt =0;
    alpha_rot = 0;
    theta_start = 0;

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

  void Estimator::setEstimator(float ins,float appoff,float th,unsigned int art)
  {
    Init =  true;

    ROT_base = Matrix33d::I();
    off = Vec3d(0,0,appoff+ins);
    
    rho_n = 155; //mm 
    rho_a = 40; //mm
    //yaw_art = 0;
    //ART_ANG= - 0 * (PI/180.0);

    firstAdd = true;

    L.push_back(ins);
    THETA.push_back(0);
    ART.push_back(0);
    TIP_t.clear();
    TIP_t.push_back(Matrix44d::FromOrientationAndTranslation(ROT_base, ROT_base*off));
    
    L.push_back(ins);
    THETA.push_back(th);
    ART.push_back(art);

    TIP_t.push_back(simulate1Step(0,th,art,ART.back(),TIP_t.back(),ins));

    TARGET.clear();
    // Aggiungere TIP
    
  }
  void Estimator::resetEstimator(void)
  {
    Init =  false;
  }
  bool Estimator::isInitialized(void)
  {
    return Init;
  }
  Matrix44d Estimator::getCurrentEstimateTIP()
  {
    WaitForSingleObject(ghMutex,INFINITE);  

     Vec3d app_tip = TIP_t.back().GetPosition();     
     Vec3d v_z= TIP_t.back().GetOrientation().Col(2);
     Matrix33d appROT = TIP_t.back().GetOrientation();

    /*if (!ART.empty())
      if (ART.back()==1)
      {
        appROT = TIP_t.back().GetOrientation();
        appROT = AxisAngle(appROT.Col(0),ART_ANG) * appROT;
        v_z= appROT.Col(2);
      }
    */
    app_tip = app_tip  + (v_z)*TIP_LENGTH;
    ReleaseMutex(ghMutex);

    return Matrix44d::FromOrientationAndTranslation(appROT, app_tip);
  }
  Matrix44d Estimator::getCurrentEstimateWRIST()
  {
    return TIP_t.back();
  }
 double Estimator::lastTHETA()
 {
   return THETA.back();
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

 

  Matrix44d Estimator::simulate1Step(double diff_l,double diff_th,unsigned int a,unsigned int prev_a, Matrix44d prevTIP,double L_n)
  {
    Vec3d app_d,tip_mm_t;
    Matrix33d ROT_t;
    //Matrix33d ROT_art;
    //ROT_art=Matrix33d::I();

    double rho;

    tip_mm_t = prevTIP.GetPosition();
    ROT_t = prevTIP.GetOrientation();

    /*if ((a == 1) && ( prev_a == 0))
    {  
      ROT_t =  AxisAngle(ROT_t.Col(2),yaw_art) * ROT_t;
      StartArt = L_n;
    }

    if ((a == 0) && ( prev_a == 1))
    {
      ROT_t =  AxisAngle(ROT_t.Col(0),ART_ANG) * ROT_t;
      ROT_t =  AxisAngle(ROT_t.Col(2),-yaw_art) * ROT_t;
    }*/
            
    if ((a==1))
    {
      rho = rho_a;
      //ROT_art=rotz(yaw_art);
    }
    else
      rho = rho_n;

    if((L_n)<(INTR_LENGTH))
    {
       rho = 1000;
    }

    /*if((L_n-StartArt)<(TIP_LENGTH))
    {
       rho = 1000;
    }*/

        
    app_d = Vec3d(0,rho*(1-cos(diff_l/rho)),rho*sin(diff_l/rho));
    tip_mm_t = tip_mm_t + ROT_t * rotz(diff_th*(PI/180)) * app_d;
    ROT_t = ROT_t * rotz(diff_th*(PI/180)) * rotx(-(diff_l/rho));

   
      

    return Matrix44d::FromOrientationAndTranslation(ROT_t, tip_mm_t);
  }

  void Estimator::updateInput(double m_l, double m_th,unsigned int a)
  {
   
   WaitForSingleObject(ghMutex,INFINITE);
   double diff_l,diff_th;
   unsigned int prev_art;
    
   diff_l = m_l - L.back();
   diff_th = m_th -THETA.back();
   prev_art = ART.back();
    
    if ((abs(diff_l)>0.001)||(abs(diff_th)>0.0002)||(prev_art != a))
    {
       if ((diff_l>=0))
       {
         L.push_back(m_l);
         THETA.push_back(m_th);
         ART.push_back(a);

         TIP_t.push_back(simulate1Step(diff_l,diff_th,a,prev_art,TIP_t.back(),m_l));
         
       }
       else
       {
         while(1)
         {
           if (L.empty())
             break;
           
           if ((m_l<L.back())&&(L.size()>1))
           {  
              L.pop_back();
              TIP_t.pop_back();
              THETA.pop_back();
              ART.pop_back();
                
              for (int i=0;i<IND.size();i++)
              {
                // Allungato
                  if (L.size()<IND[i])
                  {
                    OBS.erase(OBS.begin()+i);
                    VER.erase(VER.begin()+i);
                    IND.erase(IND.begin()+i);
                  }

              }
           }
           else
           {  // rotation along z di // voglio solo tornare indietro. Non devo ruotare per l'articolazione! PENSARCI
             
             TIP_t.push_back(simulate1Step(m_l - L.back(),m_th-THETA.back(),a,ART.back(),TIP_t.back(),m_l));
             L.push_back(m_l);
             THETA.push_back(m_th);
             ART.push_back(a);             
             
             break;
           }
           // Add point da cancellare
         }
         
      }


    }
    ReleaseMutex(ghMutex);
  }

  void Estimator::saveDataOpt()
  {
    mat dyaw = zeros(1,THETA.size()); 
    mat dins = zeros(1,L.size());
    mat point = zeros(3,OBS.size());
    mat ver = zeros(3,VER.size());
    mat R0 = zeros(3,3);
    mat offset = zeros(3,1);
    mat dart = zeros(1,ART.size());
    

    mat mrho_n = zeros(1,1);
    mat mrho_a = zeros(1,1);
    mat myaw_art = zeros(1,1);
    mat mART_ANG = zeros(1,1);
    mat malpha_rot = zeros(1,1);
    mat mtheta_start = zeros(1,1);

    mrho_n(0,0)=rho_n;
    mrho_a(0,0)=rho_a;
    malpha_rot(0,0) = alpha_rot;
    mtheta_start(0,0) = theta_start;
    //myaw_art(0,0)=yaw_art;
    //mART_ANG = ART_ANG;
    
    for (int i=0;i<L.size();i++)
    {
       dyaw(0,i) = THETA[i];
       dins(0,i) = L[i];
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
    sprintf(path, "%sALPHA_ROT.dat", basePath);
    malpha_rot.save(path, raw_ascii);
     sprintf(path, "%sTHETA_START.dat", basePath);
    mtheta_start.save(path, raw_ascii);

    
    /*sprintf(path, "%sYAW_ART.dat", basePath);
    myaw_art.save(path, raw_ascii);

    sprintf(path, "%sART_ANG.dat", basePath);
    mART_ANG.save(path, raw_ascii);*/


    
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
    int app_ind = 0;
    for (int i=0;i<PointInd.size();i++)
    {
       app_ind = PointInd(i,0);
       if (app_ind > L.size())
         app_ind = L.size();

       IND.push_back(app_ind);
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
    
    StartArt = 0;
    ROT_base = ROT_base * rotx(res(0,0)) *  roty(res(1,0)) *  rotz(res(2,0));
    
    tip_mm_t = ROT_base*off;
    ROT_t = ROT_base;

    alpha_rot = alpha_rot + res(3,0);
    Matrix33d appROT = ROT_t * rotz(theta_start);
    ROT_t = AxisAngle(appROT.Col(0),alpha_rot) * ROT_t;


    //rho_n = rho_n + res(3,0); //mm
    rho_a = rho_a + res(4,0);
    //yaw_art = yaw_art + res(5,0);
    //ART_ANG = ART_ANG + res(6,0); 
    
    TIP_t.clear();
    TIP_t.push_back(Matrix44d::FromOrientationAndTranslation(ROT_t, tip_mm_t));
    
    double diff_l,diff_th;
    Vec3d app_d; 
    double rho;
    WaitForSingleObject(ghMutex,INFINITE);
    for (int i=1;i<L.size();i++)
    {
      diff_l = L[i]-L[i-1];
      diff_th = THETA[i] - THETA[i-1];

      TIP_t.push_back(simulate1Step(diff_l,diff_th,ART[i],ART[i-1],TIP_t.back(),L[i]));

    }
    ReleaseMutex(ghMutex);
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
        appROT = TIP_t.back().GetOrientation();
        //appROT = AxisAngle(appROT.Col(0),ART_ANG) * appROT;
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



Matrix33d Estimator::AxisAngle(Vec3d axis,double angle)
{
    double c;
    double s;
    double t;

    s = sin(angle);
    c = cos(angle);
    t = 1 - c;

    double x,y,z;

    axis.normalized();
    x = axis.x;
    y = axis.y;
    z = axis.z;

    return Matrix33d(t*x*x + c, t*x*y - s*z,t*x*z + s*y, t*x*y + s*z, t*y*y + c, t*y*z - s*x, t*x*z - s*y, t*y*z + s*x, t*z*z + c );
    //return Matrix33d(1,0,0,0,ct,-st,0,st,ct);
}

bool Estimator::testArt(double L_n)
{
  if((L_n-StartArt)<(TIP_LENGTH))
    return false;
  return true;
}

Matrix44d Estimator::getCurrentEstimateTIP_trans(float horizon)
{
    Matrix44d appTIP = TIP_t.back();
    double stepL = 0.01;

    double ins = L.back()+stepL; 
    appTIP = simulate1Step(stepL,0,0,ART.back(),appTIP,ins);

    while (ins<=(horizon+L.back()))
    {
      ins = ins + stepL;
      appTIP = simulate1Step(stepL,0,0,0,appTIP,ins);
    }
     
    return appTIP;
}

void Estimator::addTarget(Vec3d t)
{
  //TARGET.clear();
  TARGET.push_front(t);
}

void Estimator::resetTarget()
{
  TARGET.clear();
}

void Estimator::simulateTask()
{
    if(!firstAdd)
      return;

    if (TIP_t.empty())
      return;
    if (TARGET.empty())
      return;

    SIMULATE_TIP.clear();

    /*for (int i=0;i<L.size();i++)
    {
      SIMULATE_TIP.push_back(TIP_t[i]);
    }*/

   

    firstAdd = false;
    SIMULATE_TIP.push_back(TIP_t.back());

    double disp = 0.01;
    Matrix44d Wrist; 
    
    double L_sim=L.back();
    double diff_th=0;
    Nf::Vec3d e,p,tar;
    Nf::Matrix33d R;
    double dt = 0.025;
    dt = dt*60;

    list <Vec3d> TARGET_app;

    TARGET_app = TARGET;
    tar = TARGET_app.front();

    while(1)
    {

     Wrist = SIMULATE_TIP.back();
     p = Wrist.GetPosition();
     R = Wrist.GetOrientation();

     e = R.Inverse()*(tar - p);
     
      if (e.z<0)
      {
        
        TARGET_app.pop_front();
        if (TARGET_app.empty())
        {
            break;
        }
        else
        {
          tar = TARGET_app.front();
        }
      }
     
     f32 d_th = atan2(-e.x, e.y);
     if (d_th<0)
         diff_th=( -350.0 * 360.0 * dt) / ( (float)FAULHABER_ONE_ROTATION1 * (float)SPUR_GEAR_RATIO1);
     else
        diff_th= ( 350.0 * 360.0 * dt) / ( (float)FAULHABER_ONE_ROTATION1 * (float)SPUR_GEAR_RATIO1);
      
      L_sim = L_sim + disp;
      SIMULATE_TIP.push_back(simulate1Step(disp,diff_th,1,1,SIMULATE_TIP.back(),L_sim));
    }


}

void Estimator::removeTarget()
{
    if (TARGET.empty())
        return;
    TARGET.pop_front();
}

void Estimator::savetheta(double t)
{
  theta_start=t;
}

/*
n = sl3dnormalize(r(1:3), epsilon);

x = n(1);
y = n(2);
z = n(3);
m = [ ...
     t*x*x + c,    t*x*y - s*z,  t*x*z + s*y; ...
     t*x*y + s*z,  t*y*y + c,    t*y*z - s*x; ...
     t*x*z - s*y,  t*y*z + s*x,  t*z*z + c ...
    ];

    */
}

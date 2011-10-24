typedef float Gfloat;

uint  const _N         = 0u;
uint  const CA         = 3u;
uint  const _C         = 6u;
uint  const Xi         = 0u;
uint  const Yi         = 1u;
uint  const Zi         = 2u;
Gfloat const COS_C_N_CA = 0.5254717f;
Gfloat const SIN_C_N_CA = 0.8508111f;
Gfloat const COS_N_CA_C = 0.3616246f;
Gfloat const SIN_N_CA_C = 0.9323238f;
Gfloat const COS_CA_C_N = 0.4415059f;
Gfloat const SIN_CA_C_N = 0.8972584f;
Gfloat const DIS_N_CA   = 1.4580010f;
Gfloat const DIS_CA_C   = 1.5232580f;
Gfloat const DIS_C_N    = 1.3286850f;

void refold_gpu_test(uint N, float const *tor, float *bb, float *b2) {
  Gfloat *SIN = new Gfloat[3*N];
  Gfloat *COS = new Gfloat[3*N];

  double t = time_highres();
  for(uint i = 0; i < 3*N; ++i) {
    SIN[i] = sin(-tor[i]);
    COS[i] = cos( tor[i]);
  }
  for(uint i = 0; i < N-1u; ++i) {
    //TR<<"init build "<<i<<std::endl;
    float *XX = ((i==0u)?b2:bb);
    uint const i9 = 9u*(i   );
    uint const i2 = 9u*(i+1u);
    XX[i9+_N+Xi] =  0.0f;
    XX[i9+_N+Yi] =  SIN_N_CA_C * DIS_N_CA;
    XX[i9+_N+Zi] = -DIS_CA_C - COS_N_CA_C * DIS_N_CA;
    XX[i9+CA+Xi] =  0.0f;
    XX[i9+CA+Yi] =  0.0f;
    XX[i9+CA+Zi] = -DIS_CA_C;
    XX[i9+_C+Xi] =  0.0f;
    XX[i9+_C+Yi] =  0.0f;
    XX[i9+_C+Zi] =  0.0f;
    b2[i2+_N+Xi] =  0.0f;
    b2[i2+_N+Yi] =  0.0f;
    b2[i2+_N+Zi] =  0.0f;
    b2[i2+CA+Xi] =  0.0f;
    b2[i2+CA+Yi] =  0.0f;
    b2[i2+CA+Zi] =  0.0f;
    b2[i2+_C+Xi] =  0.0f;
    b2[i2+_C+Yi] =  0.0f;
    b2[i2+_C+Zi] =  0.0f;

    // this first block could be merged into init!!!
    { // rot CA-C-N-CA, rot CA-N-C and drop
      Gfloat const COS_CA_C = COS[3u*i+1u];
      Gfloat const SIN_CA_C = SIN[3u*i+1u];
      Gfloat const Nx = XX[i9+_N+Xi];
      Gfloat const Ny = XX[i9+_N+Yi];
      XX[i9+_N+Xi] = mad( COS_CA_C , Nx, - SIN_CA_C * Ny );
      XX[i9+_N+Yi] = mad( SIN_CA_C , Nx, + COS_CA_C * Ny );
      // CA x/y are 0 at start, no need to rot!
    }
    { // rot CA_C_N bond angle
      Gfloat const Ny  = XX[i9+_N+Yi];
      Gfloat const Nz  = XX[i9+_N+Zi];
      Gfloat const CAy = XX[i9+CA+Yi];
      Gfloat const CAz = XX[i9+CA+Zi];
      XX[i9+_N+Yi] = mad( COS_CA_C_N ,  Ny, - SIN_CA_C_N *  Nz );
      XX[i9+_N+Zi] = mad( SIN_CA_C_N ,  Ny, + COS_CA_C_N *  Nz );
      XX[i9+CA+Yi] = mad( COS_CA_C_N , CAy, - SIN_CA_C_N * CAz );
      XX[i9+CA+Zi] = mad( SIN_CA_C_N , CAy, + COS_CA_C_N * CAz );
    }
    XX[i9+_N+Zi] -= DIS_C_N;
    XX[i9+CA+Zi] -= DIS_C_N;
    XX[i9+_C+Zi] -= DIS_C_N;
    { // rot omega2
      Gfloat const COS_C_N = COS[3u*i+2u];
      Gfloat const SIN_C_N = SIN[3u*i+2u];
      Gfloat const  Nx = XX[i9+_N+Xi];
      Gfloat const  Ny = XX[i9+_N+Yi];
      Gfloat const CAx = XX[i9+CA+Xi];
      Gfloat const CAy = XX[i9+CA+Yi];
      // Gfloat const  Cx = XX[i9+_C+Xi];
      // Gfloat const  Cy = XX[i9+_C+Yi];
      XX[i9+_N+Xi] = mad( COS_C_N ,  Nx, - SIN_C_N *  Ny );
      XX[i9+_N+Yi] = mad( SIN_C_N ,  Nx, + COS_C_N *  Ny );
      XX[i9+CA+Xi] = mad( COS_C_N , CAx, - SIN_C_N * CAy );
      XX[i9+CA+Yi] = mad( SIN_C_N , CAx, + COS_C_N * CAy );
    }
    { // rot C_N_CA angle
      Gfloat const  Ny = XX[i9+_N+Yi];
      Gfloat const  Nz = XX[i9+_N+Zi];
      Gfloat const CAy = XX[i9+CA+Yi];
      Gfloat const CAz = XX[i9+CA+Zi];
      Gfloat const  Cy = XX[i9+_C+Yi];
      Gfloat const  Cz = XX[i9+_C+Zi];
      XX[i9+_N+Yi] = mad( COS_C_N_CA ,  Ny, - SIN_C_N_CA *  Nz );
      XX[i9+_N+Zi] = mad( SIN_C_N_CA ,  Ny, + COS_C_N_CA *  Nz );
      XX[i9+CA+Yi] = mad( COS_C_N_CA , CAy, - SIN_C_N_CA * CAz );
      XX[i9+CA+Zi] = mad( SIN_C_N_CA , CAy, + COS_C_N_CA * CAz );
      XX[i9+_C+Yi] = mad( COS_C_N_CA ,  Cy, - SIN_C_N_CA *  Cz );
      XX[i9+_C+Zi] = mad( SIN_C_N_CA ,  Cy, + COS_C_N_CA *  Cz );
    }
    XX[i9+_N+Zi] -= DIS_N_CA;
    XX[i9+CA+Zi] -= DIS_N_CA;
    XX[i9+_C+Zi] -= DIS_N_CA;
    b2[i2+_N+Zi] -= DIS_N_CA;
    { // rot phi2
      Gfloat const COS_N_CA = COS[3u*i+3u];
      Gfloat const SIN_N_CA = SIN[3u*i+3u];
      Gfloat const  Nx = XX[i9+_N+Xi];
      Gfloat const  Ny = XX[i9+_N+Yi];
      Gfloat const CAx = XX[i9+CA+Xi];
      Gfloat const CAy = XX[i9+CA+Yi];
      Gfloat const  Cx = XX[i9+_C+Xi];
      Gfloat const  Cy = XX[i9+_C+Yi];
      Gfloat const N2x = b2[i2+_N+Xi];
      Gfloat const N2y = b2[i2+_N+Yi];
      XX[i9+_N+Xi] = mad( COS_N_CA ,  Nx, - SIN_N_CA *  Ny );
      XX[i9+_N+Yi] = mad( SIN_N_CA ,  Nx, + COS_N_CA *  Ny );
      XX[i9+CA+Xi] = mad( COS_N_CA , CAx, - SIN_N_CA * CAy );
      XX[i9+CA+Yi] = mad( SIN_N_CA , CAx, + COS_N_CA * CAy );
      XX[i9+_C+Xi] = mad( COS_N_CA ,  Cx, - SIN_N_CA *  Cy );
      XX[i9+_C+Yi] = mad( SIN_N_CA ,  Cx, + COS_N_CA *  Cy );
    }
    { // rot C_CA_N angle
      Gfloat const  Ny = XX[i9+_N+Yi];
      Gfloat const  Nz = XX[i9+_N+Zi];
      Gfloat const CAy = XX[i9+CA+Yi];
      Gfloat const CAz = XX[i9+CA+Zi];
      Gfloat const  Cy = XX[i9+_C+Yi];
      Gfloat const  Cz = XX[i9+_C+Zi];
      Gfloat const N2y = b2[i2+_N+Yi];
      Gfloat const N2z = b2[i2+_N+Zi];
      XX[i9+_N+Yi] = mad( COS_N_CA_C ,  Ny, - SIN_N_CA_C *  Nz );
      XX[i9+_N+Zi] = mad( SIN_N_CA_C ,  Ny, + COS_N_CA_C *  Nz );
      XX[i9+CA+Yi] = mad( COS_N_CA_C , CAy, - SIN_N_CA_C * CAz );
      XX[i9+CA+Zi] = mad( SIN_N_CA_C , CAy, + COS_N_CA_C * CAz );
      XX[i9+_C+Yi] = mad( COS_N_CA_C ,  Cy, - SIN_N_CA_C *  Cz );
      XX[i9+_C+Zi] = mad( SIN_N_CA_C ,  Cy, + COS_N_CA_C *  Cz );
      b2[i2+_N+Yi] = mad( COS_N_CA_C , N2y, - SIN_N_CA_C * N2z );
      b2[i2+_N+Zi] = mad( SIN_N_CA_C , N2y, + COS_N_CA_C * N2z );
    }
    //TR<<F(5,2,XX[i9+_N+Xi])<<" "<<F(5,2,XX[i9+_N+Yi])<<" "<<F(5,2,XX[i9+CA+Xi])<<" "<<F(5,2,XX[i9+CA+Yi])<<" "<<F(5,2,XX[i9+_C+Xi])<<" "<<F(5,2,XX[i9+_C+Yi])<<" "<<F(5,2,XX[i9+_C+Xi])<<" "<<F(5,2,XX[i9+_C+Yi])<<endl;
    XX[i9+_N+Zi] -= DIS_CA_C;
    XX[i9+CA+Zi] -= DIS_CA_C;
    XX[i9+_C+Zi] -= DIS_CA_C;
    b2[i2+_N+Zi] -= DIS_CA_C;
    b2[i2+CA+Zi] -= DIS_CA_C;

  }
  
  // for(int i = 0; i < p.n_residue(); ++i) {
  //   Pose tmp;
  //   core::pose::make_pose_from_sequence(tmp,"G",*crs,false);
  //   remove_tremini(tmp);
  //   tmp.set_xyz(AtomID(1,1),Vec(bb[9*i+0],bb[9*i+1],bb[9*i+2]));
  //   tmp.set_xyz(AtomID(2,1),Vec(bb[9*i+3],bb[9*i+4],bb[9*i+5]));
  //   tmp.set_xyz(AtomID(3,1),Vec(bb[9*i+6],bb[9*i+7],bb[9*i+8]));
  //   tmp.dump_pdb("bb"+str(i)+".pdb");
  //   tmp.set_xyz(AtomID(1,1),Vec(b2[9*i+0],b2[9*i+1],b2[9*i+2]));
  //   tmp.set_xyz(AtomID(2,1),Vec(b2[9*i+3],b2[9*i+4],b2[9*i+5]));
  //   tmp.set_xyz(AtomID(3,1),Vec(b2[9*i+6],b2[9*i+7],b2[9*i+8]));
  //   tmp.dump_pdb("b2"+str(i)+".pdb");
  // }

  for(uint c = 2u; c < N*2u-3u; c=2u*c) {
    //    TR << c << "----------------------" << endl;
    //for(uint i = c/2u; i < N-1u; i+=c) {
    for(uint j = 0; j < N; ++j) {
      uint i = (max(0,((int)j))/c)*c+c/2;
      if(j > i || i > N-2u) continue;
      uint const i9 = 9u*(i);
      MAT R;
      if(c<4u) { // skip "to" if 1st iter
        VEC const az = normalizedv(         vec(bb[i9+6u]-bb[i9+3u],bb[i9+7u]-bb[i9+4u],bb[i9+8u]-bb[i9+5u]) );
        VEC const ay = normalizedv(pproj(az,vec(bb[i9+0u]-bb[i9+3u],bb[i9+1u]-bb[i9+4u],bb[i9+2u]-bb[i9+5u])));
        R = cols(crossvv(ay,az),ay,az);
      } else {
        VEC       az = normalizedv(         vec(bb[i9+6u]-bb[i9+3u],bb[i9+7u]-bb[i9+4u],bb[i9+8u]-bb[i9+5u]) );
        VEC       ay = normalizedv(pproj(az,vec(bb[i9+0u]-bb[i9+3u],bb[i9+1u]-bb[i9+4u],bb[i9+2u]-bb[i9+5u])));
        MAT const to = cols(crossvv(ay,az),ay,az);
        az = normalizedv(                   vec(b2[i9+6u]-b2[i9+3u],b2[i9+7u]-b2[i9+4u],b2[i9+8u]-b2[i9+5u]) );
        ay = normalizedv(pproj(az,          vec(b2[i9+0u]-b2[i9+3u],b2[i9+1u]-b2[i9+4u],b2[i9+2u]-b2[i9+5u])));
        R = multmm(to,rows(crossvv(ay,az),ay,az));
      }
      VEC T(b2[i9+0u],b2[i9+1u],b2[i9+2u]);
      T = multmv(R,T);  T.x = bb[i9+0u]-T.x;  T.y = bb[i9+1u]-T.y;  T.z = bb[i9+2u]-T.z;
      uint const start = max((int)i-(int)c/2,0);
      //TR << "join " << c << " at " << i << " xform " << i <<"-"<< stop << endl;
      //TR << c << " " << stop << endl;
      //for(uint j = start; j <= i; ++j) {
      //TR << c << " " << j << " " << i << endl;
      uint const j9 = 9u*j;
      float *XX = ((j==i-c/2u&&j!=0u) ? bb : b2);
      //TR << "join " << c << " at " << i << " xform " << start <<"-"<< i <<" @ "<< j << " " << ((j==i-c/2u&&j!=0u)?"bb":"b2") << " " << endl;;
      VEC v1 = multmv(R,vec(XX[j9+0u],XX[j9+1u],XX[j9+2u]));
      VEC v2 = multmv(R,vec(XX[j9+3u],XX[j9+4u],XX[j9+5u]));
      VEC v3 = multmv(R,vec(XX[j9+6u],XX[j9+7u],XX[j9+8u]));
      XX[j9+0u]=v1.x+T.x; XX[j9+1u]=v1.y+T.y; XX[j9+2u]=v1.z+T.z;
      XX[j9+3u]=v2.x+T.x; XX[j9+4u]=v2.y+T.y; XX[j9+5u]=v2.z+T.z;
      XX[j9+6u]=v3.x+T.x; XX[j9+7u]=v3.y+T.y; XX[j9+8u]=v3.z+T.z;
      //TR << v1 << " " << v2 << " " << v3;
      //TR << endl;
      //}
    }

  }
}








//__kernel
void refold_first(
                  /*__global*/ const float *tor,
                  /*__global*/ const uint  *nres,
                  /*__global*/       float *out,
                  float *bb, float *b2
                  ){
  //  __local Gfloat bb[64*9];
  //  __local Gfloat b2[64*9];
  uint N = nres[0];
  //  for(uint i = 0; i < N-1u; ++i) {
  uint const i = get_global_id(0);
  uint const i9 = 9u*i;

  if( i < N-1u ) {
    //TR<<"init build "<<i<<std::endl;
    /*__local*/ float *XX = ((i==0u)?b2:bb);
    uint const i2 = 9u*(i+1u);
    XX[i9+_N+Xi] =  0.0f;
    XX[i9+_N+Yi] =  SIN_N_CA_C * DIS_N_CA;
    XX[i9+_N+Zi] = -DIS_CA_C - COS_N_CA_C * DIS_N_CA;
    XX[i9+CA+Xi] =  0.0f;
    XX[i9+CA+Yi] =  0.0f;
    XX[i9+CA+Zi] = -DIS_CA_C;
    XX[i9+_C+Xi] =  0.0f;
    XX[i9+_C+Yi] =  0.0f;
    XX[i9+_C+Zi] =  0.0f;
    b2[i2+_N+Xi] =  0.0f;
    b2[i2+_N+Yi] =  0.0f;
    b2[i2+_N+Zi] =  0.0f;
    b2[i2+CA+Xi] =  0.0f;
    b2[i2+CA+Yi] =  0.0f;
    b2[i2+CA+Zi] =  0.0f;
    b2[i2+_C+Xi] =  0.0f;
    b2[i2+_C+Yi] =  0.0f;
    b2[i2+_C+Zi] =  0.0f;

    // this first block could be merged into init!!!
    { // rot CA-C-N-CA, rot CA-N-C and drop
      Gfloat const COS_CA_C = cos(tor[3u*i+1u]);
      Gfloat const SIN_CA_C = sin(tor[3u*i+1u]);
      Gfloat const Nx = XX[i9+_N+Xi];
      Gfloat const Ny = XX[i9+_N+Yi];
      XX[i9+_N+Xi] = mad(  COS_CA_C , Nx, + SIN_CA_C * Ny );
      XX[i9+_N+Yi] = mad( -SIN_CA_C , Nx, + COS_CA_C * Ny );
      // CA x/y are 0 at start, no need to rot!
    }
    { // rot CA_C_N bond angle
      Gfloat const Ny  = XX[i9+_N+Yi];
      Gfloat const Nz  = XX[i9+_N+Zi];
      Gfloat const CAy = XX[i9+CA+Yi];
      Gfloat const CAz = XX[i9+CA+Zi];
      XX[i9+_N+Yi] = mad( COS_CA_C_N ,  Ny, -SIN_CA_C_N *  Nz );
      XX[i9+_N+Zi] = mad( SIN_CA_C_N ,  Ny,  COS_CA_C_N *  Nz );
      XX[i9+CA+Yi] = mad( COS_CA_C_N , CAy, -SIN_CA_C_N * CAz );
      XX[i9+CA+Zi] = mad( SIN_CA_C_N , CAy,  COS_CA_C_N * CAz );
    }
    XX[i9+_N+Zi] -= DIS_C_N;
    XX[i9+CA+Zi] -= DIS_C_N;
    XX[i9+_C+Zi] -= DIS_C_N;
    { // rot omega2
      Gfloat const COS_C_N = cos(tor[3u*i+2u]);
      Gfloat const SIN_C_N = sin(tor[3u*i+2u]);
      Gfloat const  Nx = XX[i9+_N+Xi];
      Gfloat const  Ny = XX[i9+_N+Yi];
      Gfloat const CAx = XX[i9+CA+Xi];
      Gfloat const CAy = XX[i9+CA+Yi];
      // Gfloat const  Cx = XX[i9+_C+Xi];
      // Gfloat const  Cy = XX[i9+_C+Yi];
      XX[i9+_N+Xi] = mad(  COS_C_N ,  Nx, + SIN_C_N *  Ny );
      XX[i9+_N+Yi] = mad( -SIN_C_N ,  Nx, + COS_C_N *  Ny );
      XX[i9+CA+Xi] = mad(  COS_C_N , CAx, + SIN_C_N * CAy );
      XX[i9+CA+Yi] = mad( -SIN_C_N , CAx, + COS_C_N * CAy );
    }
    { // rot C_N_CA angle
      Gfloat const  Ny = XX[i9+_N+Yi];
      Gfloat const  Nz = XX[i9+_N+Zi];
      Gfloat const CAy = XX[i9+CA+Yi];
      Gfloat const CAz = XX[i9+CA+Zi];
      Gfloat const  Cy = XX[i9+_C+Yi];
      Gfloat const  Cz = XX[i9+_C+Zi];
      XX[i9+_N+Yi] = mad( COS_C_N_CA ,  Ny, -SIN_C_N_CA *  Nz );
      XX[i9+_N+Zi] = mad( SIN_C_N_CA ,  Ny,  COS_C_N_CA *  Nz );
      XX[i9+CA+Yi] = mad( COS_C_N_CA , CAy, -SIN_C_N_CA * CAz );
      XX[i9+CA+Zi] = mad( SIN_C_N_CA , CAy,  COS_C_N_CA * CAz );
      XX[i9+_C+Yi] = mad( COS_C_N_CA ,  Cy, -SIN_C_N_CA *  Cz );
      XX[i9+_C+Zi] = mad( SIN_C_N_CA ,  Cy,  COS_C_N_CA *  Cz );
    }
    XX[i9+_N+Zi] -= DIS_N_CA;
    XX[i9+CA+Zi] -= DIS_N_CA;
    XX[i9+_C+Zi] -= DIS_N_CA;
    b2[i2+_N+Zi] -= DIS_N_CA;
    { // rot phi2
      Gfloat const COS_N_CA = cos(tor[3u*i+3u]);
      Gfloat const SIN_N_CA = sin(tor[3u*i+3u]);
      Gfloat const  Nx = XX[i9+_N+Xi];
      Gfloat const  Ny = XX[i9+_N+Yi];
      Gfloat const CAx = XX[i9+CA+Xi];
      Gfloat const CAy = XX[i9+CA+Yi];
      Gfloat const  Cx = XX[i9+_C+Xi];
      Gfloat const  Cy = XX[i9+_C+Yi];
      Gfloat const N2x = b2[i2+_N+Xi];
      Gfloat const N2y = b2[i2+_N+Yi];
      XX[i9+_N+Xi] = mad(  COS_N_CA ,  Nx, SIN_N_CA *  Ny );
      XX[i9+_N+Yi] = mad( -SIN_N_CA ,  Nx, COS_N_CA *  Ny );
      XX[i9+CA+Xi] = mad(  COS_N_CA , CAx, SIN_N_CA * CAy );
      XX[i9+CA+Yi] = mad( -SIN_N_CA , CAx, COS_N_CA * CAy );
      XX[i9+_C+Xi] = mad(  COS_N_CA ,  Cx, SIN_N_CA *  Cy );
      XX[i9+_C+Yi] = mad( -SIN_N_CA ,  Cx, COS_N_CA *  Cy );
    }
    { // rot C_CA_N angle
      Gfloat const  Ny = XX[i9+_N+Yi];
      Gfloat const  Nz = XX[i9+_N+Zi];
      Gfloat const CAy = XX[i9+CA+Yi];
      Gfloat const CAz = XX[i9+CA+Zi];
      Gfloat const  Cy = XX[i9+_C+Yi];
      Gfloat const  Cz = XX[i9+_C+Zi];
      Gfloat const N2y = b2[i2+_N+Yi];
      Gfloat const N2z = b2[i2+_N+Zi];
      XX[i9+_N+Yi] = mad( COS_N_CA_C ,  Ny, -SIN_N_CA_C *  Nz );
      XX[i9+_N+Zi] = mad( SIN_N_CA_C ,  Ny,  COS_N_CA_C *  Nz );
      XX[i9+CA+Yi] = mad( COS_N_CA_C , CAy, -SIN_N_CA_C * CAz );
      XX[i9+CA+Zi] = mad( SIN_N_CA_C , CAy,  COS_N_CA_C * CAz );
      XX[i9+_C+Yi] = mad( COS_N_CA_C ,  Cy, -SIN_N_CA_C *  Cz );
      XX[i9+_C+Zi] = mad( SIN_N_CA_C ,  Cy,  COS_N_CA_C *  Cz );
      b2[i2+_N+Yi] = mad( COS_N_CA_C , N2y, -SIN_N_CA_C * N2z );
      b2[i2+_N+Zi] = mad( SIN_N_CA_C , N2y,  COS_N_CA_C * N2z );
    }
    //TR<<F(5,2,XX[i9+_N+Xi])<<" "<<F(5,2,XX[i9+_N+Yi])<<" "<<F(5,2,XX[i9+CA+Xi])<<" "<<F(5,2,XX[i9+CA+Yi])<<" "<<F(5,2,XX[i9+_C+Xi])<<" "<<F(5,2,XX[i9+_C+Yi])<<" "<<F(5,2,XX[i9+_C+Xi])<<" "<<F(5,2,XX[i9+_C+Yi])<<endl;
    XX[i9+_N+Zi] -= DIS_CA_C;
    XX[i9+CA+Zi] -= DIS_CA_C;
    XX[i9+_C+Zi] -= DIS_CA_C;
    b2[i2+_N+Zi] -= DIS_CA_C;
    b2[i2+CA+Zi] -= DIS_CA_C;

  }
}


void refold_second(
                   /*__global*/ const float *tor,
                   /*__global*/ const uint  *nres,
                   /*__global*/       float *out,
                   uint c,
                   float *bb, float *b2
                   ){
  uint N = nres[0];

  //for(uint c = 2u; c < N*2u-3u; c=2u*c) {
  uint j = get_global_id(0);
  if( j < N ) {
    uint i = (max(0,((int)j))/c)*c+c/2;
    if(j > i || i > N-2u) return; //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    uint const i9 = 9u*(i);
    struct MAT R;
    if(c<4u) { // skip "to" if 1st iter
      struct VEC const az = normalizedv(         vec(bb[i9+6u]-bb[i9+3u],bb[i9+7u]-bb[i9+4u],bb[i9+8u]-bb[i9+5u]) );
      struct VEC const ay = normalizedv(pproj(az,vec(bb[i9+0u]-bb[i9+3u],bb[i9+1u]-bb[i9+4u],bb[i9+2u]-bb[i9+5u])));
      R = cols(crossvv(ay,az),ay,az);
    } else {
      struct VEC       az = normalizedv(         vec(bb[i9+6u]-bb[i9+3u],bb[i9+7u]-bb[i9+4u],bb[i9+8u]-bb[i9+5u]) );
      struct VEC       ay = normalizedv(pproj(az,vec(bb[i9+0u]-bb[i9+3u],bb[i9+1u]-bb[i9+4u],bb[i9+2u]-bb[i9+5u])));
      struct MAT const to = cols(crossvv(ay,az),ay,az);
      az = normalizedv(                   vec(b2[i9+6u]-b2[i9+3u],b2[i9+7u]-b2[i9+4u],b2[i9+8u]-b2[i9+5u]) );
      ay = normalizedv(pproj(az,          vec(b2[i9+0u]-b2[i9+3u],b2[i9+1u]-b2[i9+4u],b2[i9+2u]-b2[i9+5u])));
      R = multmm(to,rows(crossvv(ay,az),ay,az));
    }
    struct VEC T = vec(b2[i9+0u],b2[i9+1u],b2[i9+2u]);
    T = multmv(R,T);  T.x = bb[i9+0u]-T.x;  T.y = bb[i9+1u]-T.y;  T.z = bb[i9+2u]-T.z;
    uint const start = max((int)i-(int)c/2,0);
    //barrier(CLK_LOCAL_MEM_FENCE);
    uint const j9 = 9u*j;
    /*__local*/ float *XX = ((j==i-c/2u&&j!=0u) ? bb : b2);
    //TR << "join " << c << " at " << i << " xform " << start <<"-"<< i <<" @ "<< j << " " << ((j==i-c/2u&&j!=0u)?"bb":"b2") << " " << endl;;
    struct VEC v1 = multmv(R,vec(XX[j9+0u],XX[j9+1u],XX[j9+2u]));
    struct VEC v2 = multmv(R,vec(XX[j9+3u],XX[j9+4u],XX[j9+5u]));
    struct VEC v3 = multmv(R,vec(XX[j9+6u],XX[j9+7u],XX[j9+8u]));
    XX[j9+0u]=v1.x+T.x; XX[j9+1u]=v1.y+T.y; XX[j9+2u]=v1.z+T.z;
    XX[j9+3u]=v2.x+T.x; XX[j9+4u]=v2.y+T.y; XX[j9+5u]=v2.z+T.z;
    XX[j9+6u]=v3.x+T.x; XX[j9+7u]=v3.y+T.y; XX[j9+8u]=v3.z+T.z;
  }
}

void refold_third(
                  /*__global*/ const float *tor,
                  /*__global*/ const uint  *nres,
                  /*__global*/       float *out,
                  float *bb, float *b2
                  ){
  uint const i9 = 9u*get_global_id(0);
  //barrier(CLK_LOCAL_MEM_FENCE);    uint i9 = get_global_id(0)*9u;
  out[i9+0u] = b2[i9+0u];
  out[i9+1u] = b2[i9+1u];
  out[i9+2u] = b2[i9+2u];
  out[i9+3u] = b2[i9+3u];
  out[i9+4u] = b2[i9+4u];
  out[i9+5u] = b2[i9+5u];
  out[i9+6u] = b2[i9+6u];
  out[i9+7u] = b2[i9+7u];
  out[i9+8u] = b2[i9+8u];
}

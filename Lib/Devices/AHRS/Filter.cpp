/* This file is part of the Razor AHRS Firmware */
#include <math.h>
#include "Filter.hpp"

#define GRAVITY 256.0
#define Kp_YAW 1.2f
#define Ki_YAW 0.00002f
// XXX: Be more accurate?
#define G_Dt 0.01

namespace AHRS
{
    float constrain(float a, float m, float M)
    {
        if (a < m) return m;
        else if (a > M) return M;
        else return a;
    }

    // Computes the dot product of two vectors
    float Filter::Vector_Dot_Product(const float v1[3], const float v2[3])
    {
        float result = 0;

        for(int c = 0; c < 3; c++)
        {
            result += v1[c] * v2[c];
        }

        return result; 
    }

    // Computes the cross product of two vectors
    // out has to different from v1 and v2 (no in-place)!
    void Filter::Vector_Cross_Product(float out[3], const float v1[3], const float v2[3])
    {
        out[0] = (v1[1] * v2[2]) - (v1[2] * v2[1]);
        out[1] = (v1[2] * v2[0]) - (v1[0] * v2[2]);
        out[2] = (v1[0] * v2[1]) - (v1[1] * v2[0]);
    }

    // Multiply the vector by a scalar
    void Filter::Vector_Scale(float out[3], const float v[3], float scale)
    {
        for(int c = 0; c < 3; c++)
        {
            out[c] = v[c] * scale; 
        }
    }

    // Adds two vectors
    void Filter::Vector_Add(float out[3], const float v1[3], const float v2[3])
    {
        for(int c = 0; c < 3; c++)
        {
            out[c] = v1[c] + v2[c];
        }
    }

    // Multiply two 3x3 matrices: out = a * b
    // out has to different from a and b (no in-place)!
    void Filter::Matrix_Multiply(const float a[3][3], const float b[3][3], float out[3][3])
    {
        for(int x = 0; x < 3; x++)  // rows
        {
            for(int y = 0; y < 3; y++)  // columns
            {
                out[x][y] = a[x][0] * b[0][y] + a[x][1] * b[1][y] + a[x][2] * b[2][y];
            }
        }
    }

    // Multiply 3x3 matrix with vector: out = a * b
    // out has to different from b (no in-place)!
    void Filter::Matrix_Vector_Multiply(const float a[3][3], const float b[3], float out[3])
    {
        for(int x = 0; x < 3; x++)
        {
            out[x] = a[x][0] * b[0] + a[x][1] * b[1] + a[x][2] * b[2];
        }
    }

    // Init rotation matrix using euler angles
    void Filter::init_rotation_matrix(float m[3][3], float yaw, float pitch, float roll)
    {
        float c1 = cos(roll);
        float s1 = sin(roll);
        float c2 = cos(pitch);
        float s2 = sin(pitch);
        float c3 = cos(yaw);
        float s3 = sin(yaw);

        // Euler angles, right-handed, intrinsic, XYZ convention
        // (which means: rotate around body axes Z, Y', X'') 
        m[0][0] = c2 * c3;
        m[0][1] = c3 * s1 * s2 - c1 * s3;
        m[0][2] = s1 * s3 + c1 * c3 * s2;

        m[1][0] = c2 * s3;
        m[1][1] = c1 * c3 + s1 * s2 * s3;
        m[1][2] = c1 * s2 * s3 - c3 * s1;

        m[2][0] = -s2;
        m[2][1] = c2 * s1;
        m[2][2] = c1 * c2;
    }
    /* This file is part of the Razor AHRS Firmware */

    // DCM algorithm

    /**************************************************/
    void Filter::Normalize(void)
    {
        float error=0;
        float temporary[3][3];
        float renorm=0;

        error= -Vector_Dot_Product(&DCM_Matrix[0][0],&DCM_Matrix[1][0])*.5; //eq.19

        Vector_Scale(&temporary[0][0], &DCM_Matrix[1][0], error); //eq.19
        Vector_Scale(&temporary[1][0], &DCM_Matrix[0][0], error); //eq.19

        Vector_Add(&temporary[0][0], &temporary[0][0], &DCM_Matrix[0][0]);//eq.19
        Vector_Add(&temporary[1][0], &temporary[1][0], &DCM_Matrix[1][0]);//eq.19

        Vector_Cross_Product(&temporary[2][0],&temporary[0][0],&temporary[1][0]); // c= a x b //eq.20

        renorm= .5 *(3 - Vector_Dot_Product(&temporary[0][0],&temporary[0][0])); //eq.21
        Vector_Scale(&DCM_Matrix[0][0], &temporary[0][0], renorm);

        renorm= .5 *(3 - Vector_Dot_Product(&temporary[1][0],&temporary[1][0])); //eq.21
        Vector_Scale(&DCM_Matrix[1][0], &temporary[1][0], renorm);

        renorm= .5 *(3 - Vector_Dot_Product(&temporary[2][0],&temporary[2][0])); //eq.21
        Vector_Scale(&DCM_Matrix[2][0], &temporary[2][0], renorm);
    }

    /**************************************************/
    void Filter::Drift_correction(void)
    {
        float mag_heading_x;
        float mag_heading_y;
        float errorCourse;
        //Compensation the Roll, Pitch and Yaw drift. 
        static float Scaled_Omega_P[3];
        static float Scaled_Omega_I[3];
        float Accel_magnitude;
        float Accel_weight;


        //*****Roll and Pitch***************

        // Calculate the magnitude of the accelerometer vector
        Accel_magnitude = sqrt(Accel_Vector[0]*Accel_Vector[0] + Accel_Vector[1]*Accel_Vector[1] + Accel_Vector[2]*Accel_Vector[2]);
        Accel_magnitude = Accel_magnitude/GRAVITY; // Scale to gravity.
        // Dynamic weighting of accelerometer info (reliability filter)
        // Weight for accelerometer info (<0.5G = 0.0, 1G = 1.0 , >1.5G = 0.0)
        Accel_weight = constrain(1 - 2*fabs(1 - Accel_magnitude),0,1);  //  

        Vector_Cross_Product(&errorRollPitch[0],&Accel_Vector[0],&DCM_Matrix[2][0]); //adjust the ground of reference
        Vector_Scale(&Omega_P[0],&errorRollPitch[0],Kp_rollPitch*Accel_weight);

        Vector_Scale(&Scaled_Omega_I[0],&errorRollPitch[0],Ki_rollPitch*Accel_weight);
        Vector_Add(Omega_I,Omega_I,Scaled_Omega_I);     

        //*****YAW***************
        // We make the gyro YAW drift correction based on compass magnetic heading

        if (useCompass) {
            mag_heading_x = cos(magnHeading);
            mag_heading_y = sin(magnHeading);
            errorCourse=(DCM_Matrix[0][0]*mag_heading_y) - (DCM_Matrix[1][0]*mag_heading_x);  //Calculating YAW error
            Vector_Scale(errorYaw,&DCM_Matrix[2][0],errorCourse); //Applys the yaw correction to the XYZ rotation of the aircraft, depeding the position.

            Vector_Scale(&Scaled_Omega_P[0],&errorYaw[0],Kp_YAW);//.01proportional of YAW.
            Vector_Add(Omega_P,Omega_P,Scaled_Omega_P);//Adding  Proportional.

            Vector_Scale(&Scaled_Omega_I[0],&errorYaw[0],Ki_YAW);//.00001Integrator
            Vector_Add(Omega_I,Omega_I,Scaled_Omega_I);//adding integrator to the Omega_I
        }
    }

    void Filter::Matrix_update(void)
    {
        Gyro_Vector[0]=gyro[0]; //gyro x roll
        Gyro_Vector[1]=gyro[1]; //gyro y pitch
        Gyro_Vector[2]=gyro[2]; //gyro z yaw

        Accel_Vector[0]=accel[0]*GRAVITY;
        Accel_Vector[1]=accel[1]*GRAVITY;
        Accel_Vector[2]=accel[2]*GRAVITY;

        Vector_Add(&Omega[0], &Gyro_Vector[0], &Omega_I[0]);  //adding proportional term
        Vector_Add(&Omega_Vector[0], &Omega[0], &Omega_P[0]); //adding Integrator term

#if DEBUG__NO_DRIFT_CORRECTION == true // Do not use drift correction
        Update_Matrix[0][0]=0;
        Update_Matrix[0][1]=-G_Dt*Gyro_Vector[2];//-z
        Update_Matrix[0][2]=G_Dt*Gyro_Vector[1];//y
        Update_Matrix[1][0]=G_Dt*Gyro_Vector[2];//z
        Update_Matrix[1][1]=0;
        Update_Matrix[1][2]=-G_Dt*Gyro_Vector[0];
        Update_Matrix[2][0]=-G_Dt*Gyro_Vector[1];
        Update_Matrix[2][1]=G_Dt*Gyro_Vector[0];
        Update_Matrix[2][2]=0;
#else // Use drift correction
        Update_Matrix[0][0]=0;
        Update_Matrix[0][1]=-G_Dt*Omega_Vector[2];//-z
        Update_Matrix[0][2]=G_Dt*Omega_Vector[1];//y
        Update_Matrix[1][0]=G_Dt*Omega_Vector[2];//z
        Update_Matrix[1][1]=0;
        Update_Matrix[1][2]=-G_Dt*Omega_Vector[0];//-x
        Update_Matrix[2][0]=-G_Dt*Omega_Vector[1];//-y
        Update_Matrix[2][1]=G_Dt*Omega_Vector[0];//x
        Update_Matrix[2][2]=0;
#endif

        Matrix_Multiply(DCM_Matrix,Update_Matrix,Temporary_Matrix); //a*b=c

        for(int x=0; x<3; x++) //Matrix Addition (update)
        {
            for(int y=0; y<3; y++)
            {
                DCM_Matrix[x][y]+=Temporary_Matrix[x][y];
            } 
        }
    }

    Eigen::Matrix3d Filter::getMatrix()
    {
        Eigen::Matrix3d m;
        for (int r=0; r<3; r++) {
            for (int c=0; c<3; c++) {
                if (invert && c<2) {
                    m(r, c) = -DCM_Matrix[r][c];
                } else {
                    m(r, c) = DCM_Matrix[r][c];
                }
            }
        }

        return m;
    }

    void Filter::Euler_angles(void)
    {
        auto m = getMatrix();

        pitch = -asin(m(2,0));
        roll = atan2(m(2,1),m(2,2));
        yaw = atan2(m(1,0),m(0,0));
    }

    void Filter::Compass_Heading()
    {
        float mag_x;
        float mag_y;
        float cos_roll;
        float sin_roll;
        float cos_pitch;
        float sin_pitch;

        cos_roll = cos(roll);
        sin_roll = sin(roll);
        cos_pitch = cos(pitch);
        sin_pitch = sin(pitch);

        // Tilt compensated magnetic field X
        mag_x = magnetom[0] * cos_pitch + magnetom[1] * sin_roll * sin_pitch + magnetom[2] * cos_roll * sin_pitch;
        // Tilt compensated magnetic field Y
        mag_y = magnetom[1] * cos_roll - magnetom[2] * sin_roll;
        // Magnetic Heading
        magnHeading = atan2(-mag_y, mag_x);
    }

    Filter::Filter(bool useCompass)
        : useCompass(useCompass)
    {
        yaw = 0;
        pitch = 0;
        roll = 0;
        gyroYaw = 0;
        invert = false;
    }

    void Filter::update()
    {
        // Updating gyro Yaw
        gyroYaw += gyro[2]*G_Dt;
        while (gyroYaw > M_PI) gyroYaw -= 2*M_PI;
        while (gyroYaw < -M_PI) gyroYaw += 2*M_PI;

        // Updating magn azimuth
        magnAzimuth = atan2(magnetom[2], magnetom[0]);

        if (useCompass) {
            Compass_Heading();
        }
        Matrix_update();
        Normalize();
        Drift_correction();
        Euler_angles();
    }

}

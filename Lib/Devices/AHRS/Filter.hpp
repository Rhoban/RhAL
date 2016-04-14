#pragma once

namespace AHRS
{
    class Filter
    {
        public:
            Filter(bool useCompass=false);

            void update();

            // Use compass?
            bool useCompass;

            // Sensor values
            float accel[3];
            float magnetom[3];
            float gyro[3];

            // Euler angles
            float yaw;
            float pitch;
            float roll;
            float gyroYaw;

        protected:
            // DCM variables
            float MAG_Heading = 0;
            float Accel_Vector[3]= {0, 0, 0}; // Store the acceleration in a vector
            float Gyro_Vector[3]= {0, 0, 0}; // Store the gyros turn rate in a vector
            float Omega_Vector[3]= {0, 0, 0}; // Corrected Gyro_Vector data
            float Omega_P[3]= {0, 0, 0}; // Omega Proportional correction
            float Omega_I[3]= {0, 0, 0}; // Omega Integrator
            float Omega[3]= {0, 0, 0};
            float errorRollPitch[3] = {0, 0, 0};
            float errorYaw[3] = {0, 0, 0};
            float DCM_Matrix[3][3] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};
            float Update_Matrix[3][3] = {{0, 1, 2}, {3, 4, 5}, {6, 7, 8}};
            float Temporary_Matrix[3][3] = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}};

            float Vector_Dot_Product(const float v1[3], const float v2[3]);
            void Vector_Cross_Product(float out[3], const float v1[3], const float v2[3]);
            void Vector_Scale(float out[3], const float v[3], float scale);
            void Vector_Add(float out[3], const float v1[3], const float v2[3]);
            void Matrix_Multiply(const float a[3][3], const float b[3][3], float out[3][3]);
            void Matrix_Vector_Multiply(const float a[3][3], const float b[3], float out[3]);
            void init_rotation_matrix(float m[3][3], float yaw, float pitch, float roll);
            void Normalize(void);
            void Drift_correction(void);
            void Matrix_update(void);
            void Euler_angles(void);
            void Compass_Heading();
    };
}

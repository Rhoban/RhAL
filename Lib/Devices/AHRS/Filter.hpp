#pragma once

#include <Eigen/Dense>

namespace AHRS
{
class Filter
{
public:
  Filter(bool useCompass = false);

  void update();

  // Get DCM matrix
  Eigen::Matrix3d getMatrix();

  // Use compass?
  bool useCompass;

  // Sensor values
  double accel[3];
  double magnetom[3];
  double gyro[3];

  // Euler angles
  double yaw;
  double pitch;
  double roll;
  double gyroYaw;

  // Magnetometer
  double magnAzimuth;
  double magnHeading;

  // Kp & Ki for filter
  double Kp_rollPitch;
  double Ki_rollPitch;

  // Invert? (X Axis backward)
  bool invertX, invertY, invertZ;

  // Tick number
  int tick;

protected:
  // DCM variables
  double MAG_Heading = 0;
  double Accel_Vector[3] = { 0, 0, 0 };  // Store the acceleration in a vector
  double Gyro_Vector[3] = { 0, 0, 0 };   // Store the gyros turn rate in a vector
  double Omega_Vector[3] = { 0, 0, 0 };  // Corrected Gyro_Vector data
  double Omega_P[3] = { 0, 0, 0 };       // Omega Proportional correction
  double Omega_I[3] = { 0, 0, 0 };       // Omega Integrator
  double Omega[3] = { 0, 0, 0 };
  double errorRollPitch[3] = { 0, 0, 0 };
  double errorYaw[3] = { 0, 0, 0 };
  double DCM_Matrix[3][3] = { { 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 } };
  double Update_Matrix[3][3] = { { 0, 1, 2 }, { 3, 4, 5 }, { 6, 7, 8 } };
  double Temporary_Matrix[3][3] = { { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } };

  double Vector_Dot_Product(const double v1[3], const double v2[3]);
  void Vector_Cross_Product(double out[3], const double v1[3], const double v2[3]);
  void Vector_Scale(double out[3], const double v[3], double scale);
  void Vector_Add(double out[3], const double v1[3], const double v2[3]);
  void Matrix_Multiply(const double a[3][3], const double b[3][3], double out[3][3]);
  void Matrix_Vector_Multiply(const double a[3][3], const double b[3], double out[3]);
  void init_rotation_matrix(double m[3][3], double yaw, double pitch, double roll);
  void Normalize(void);
  void Drift_correction(void);
  void Matrix_update(void);
  void Euler_angles(void);
  void Compass_Heading();
};
}  // namespace AHRS

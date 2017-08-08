#include "kinematics.hpp"
#include "hebi_kinematic_parameters.h"

namespace hebi {
namespace kinematics {

////////////////////////// Kinematic Bodies
//
std::unique_ptr<KinematicBody> KinematicBody::createX5()
{
  auto tmp = hebiActuatorCreate(
    hebiKinematicParametersX5.com,
    hebiKinematicParametersX5.input_to_joint,
    hebiKinematicParametersX5.joint_rotation_axis,
    hebiKinematicParametersX5.joint_to_output);
  return std::unique_ptr<KinematicBody>(new KinematicBody(tmp));
}

std::unique_ptr<KinematicBody> KinematicBody::createX5Link(float length, float twist)
{
  auto link_params = hebiKinematicParametersX5Link(length, twist);
  auto tmp = hebiStaticBodyCreate(link_params.com, 1, link_params.output);
  return std::unique_ptr<KinematicBody>(new KinematicBody(tmp));
}

std::unique_ptr<KinematicBody> KinematicBody::createGenericLink(const Eigen::Vector3f& com, const Eigen::Matrix4f& output)
{
  float c_com[3];
  float c_output[16];
  Map<Matrix<float, 3, 1> > tmp_com(c_com);
  Map<Matrix<float, 4, 4, RowMajor> > tmp_output(c_output);
  tmp_com = com;
  tmp_output = output;
  
  auto tmp = hebiStaticBodyCreate(c_com, 1, c_output);
  return std::unique_ptr<KinematicBody>(new KinematicBody(tmp));
}

////////////////////////// Kinematics

Kinematics::Kinematics()
  : internal_(hebiKinematicsCreate())
{
}

Kinematics::~Kinematics() noexcept
{
  hebiKinematicsRelease(internal_);
}

void Kinematics::setBaseFrame(const Eigen::Matrix4f& base_frame)
{
  // Put data into an array
  float transform[16];
  Map<Matrix<float, 4, 4, RowMajor> > tmp(transform);
  tmp = base_frame;
  hebiKinematicsSetBaseFrame(internal_, transform);
}

Eigen::Matrix4f Kinematics::getBaseFrame() const
{
  // Get the data into an array
  float transform[16];
  hebiKinematicsGetBaseFrame(internal_, transform);

  // Copy out data
  Map<const Matrix<float, 4, 4, RowMajor> > tmp(transform);
  Eigen::Matrix4f res;
  res = tmp;
  return res;
}

size_t Kinematics::getFrameCount(HebiFrameType frame_type) const
{
  int res = hebiKinematicsGetNumberOfFrames(internal_, frame_type);
  if (res < 0)
    return 0;
  return (size_t)res;
}

size_t Kinematics::getDoFCount() const
{
  int res = hebiKinematicsGetNumberOfDoFs(internal_);
  if (res < 0)
    return 0;
  return (size_t)res;
}

// TODO: handle trees/etc by passing in parent object here, and output index
bool Kinematics::addBody(std::unique_ptr<KinematicBody> body)
{
  bool was_added = (hebiKinematicsAddBody(internal_, nullptr, 0, body->getInternal()) == 0);
  if (was_added)
    body->consume();
  // Destroy the C++ wrapper no matter what (this should automatically happen
  // anyway as we go out of scope...)
  body.reset(nullptr);
  return was_added;
}

void Kinematics::getForwardKinematics(HebiFrameType frame_type, const Eigen::VectorXd& positions, Matrix4fVector& frames) const
{
  getFK(frame_type, positions, frames);
}
void Kinematics::getFK(HebiFrameType frame_type, const Eigen::VectorXd& positions, Matrix4fVector& frames) const
{
  // Put data into an array
  double* positions_array = new double[positions.size()];
  {
    Map<Eigen::VectorXd> tmp(positions_array, positions.size());
    tmp = positions;
  }
  int num_frames = getFrameCount(frame_type);
  float* frame_array = new float[16 * num_frames];
  // Get data from C API
  hebiKinematicsGetForwardKinematics(internal_, frame_type, (double*)positions_array, (float*) frame_array);
  delete[] positions_array;
  // Copy into vector of matrices passed in
  frames.resize(num_frames);
  for (int i = 0; i < num_frames; ++i)
  {
    Map<Matrix<float, 4, 4, RowMajor> > tmp(frame_array + i * 16);
    frames[i] = tmp;
  }
  delete[] frame_array;
}

void Kinematics::getEndEffector(HebiFrameType frame_type, const Eigen::VectorXd& positions, Eigen::Matrix4f& transform) const
{
  // Put data into an array
  double* positions_array = new double[positions.size()];
  {
    Map<Eigen::VectorXd> tmp(positions_array, positions.size());
    tmp = positions;
  }

  float transform_array[16];
  hebiKinematicsGetEndEffector(internal_, frame_type, positions_array, transform_array);
  delete[] positions_array;
  {
    Map<Matrix<float, 4, 4, RowMajor> > tmp(transform_array);
    transform = tmp;
  }
}

void Kinematics::solveInverseKinematics(const Eigen::Vector3f& target_xyz, const Eigen::VectorXd& initial_positions, Eigen::VectorXd& result) const
{
  solveIK(target_xyz, initial_positions, result);
}
void Kinematics::solveIK(const Eigen::Vector3f& target_xyz, const Eigen::VectorXd& initial_positions, Eigen::VectorXd& result) const
{
  // Put data into an array
  float* target_xyz_array = new float[target_xyz.size()];
  {
    Map<Eigen::Vector3f> tmp(target_xyz_array);
    tmp = target_xyz;
  }
  double* positions_array = new double[initial_positions.size()];
  {
    Map<Eigen::VectorXd> tmp(positions_array, initial_positions.size());
    tmp = initial_positions;
  }
  double* result_array = new double[initial_positions.size()];
  hebiKinematicsSolveIK(internal_, (const float*)target_xyz_array, (const double*)positions_array, (double*) result_array);
  delete[] target_xyz_array;
  delete[] positions_array;

  {
    Map<Eigen::VectorXd> tmp(result_array, initial_positions.size());
    result = tmp;
  }
  delete[] result_array;
}


void Kinematics::getJacobians(HebiFrameType frame_type, const Eigen::VectorXd& positions, MatrixXfVector& jacobians) const
{
  getJ(frame_type, positions, jacobians);
}
void Kinematics::getJ(HebiFrameType frame_type, const Eigen::VectorXd& positions, MatrixXfVector& jacobians) const
{
  // Put data into an array
  double* positions_array = new double[positions.size()];
  {
    Map<Eigen::VectorXd> tmp(positions_array, positions.size());
    tmp = positions;
  }

  int num_frames = getFrameCount(frame_type);
  int num_dofs = positions.size();
  int rows = 6 * num_frames;
  int cols = num_dofs;
  float* jacobians_array = new float[rows * cols];
  hebiKinematicsGetJacobians(internal_, frame_type, (const double*) positions_array, (float*) jacobians_array);
  delete[] positions_array;
  jacobians.resize(num_frames);
  for (int i = 0; i < num_frames; ++i)
  {
    Map<Matrix<float, Dynamic, Dynamic, RowMajor> > tmp(jacobians_array + i * cols * 6, 6, cols);
    jacobians[i] = tmp;
  }
  delete[] jacobians_array;
}
void Kinematics::getJacobianEndEffector(HebiFrameType frame_type, const Eigen::VectorXd& positions, Eigen::MatrixXf& jacobian) const
{
  getJEndEffector(frame_type, positions, jacobian);
}
void Kinematics::getJEndEffector(HebiFrameType frame_type, const Eigen::VectorXd& positions, Eigen::MatrixXf& jacobian) const
{
  MatrixXfVector tmp_jacobians;
  getJacobians(frame_type, positions, tmp_jacobians);

  // NOTE: could make this more efficient by writing additional lib function
  // for this, instead of tossing away almost everything from the full one!

  int num_dofs = positions.size();
  jacobian.resize(6, num_dofs);
  jacobian = *tmp_jacobians.rbegin();
}

} // namespace kinematics
} // namespace hebi

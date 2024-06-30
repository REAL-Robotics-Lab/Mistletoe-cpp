#include <cmath>
#include <Eigen/Dense>

using namespace Eigen;
using namespace std;

double revs_to_radians(double angle_rev) {
    M_PI * 2 * angle_rev;
}

Matrix<double, 3, 3> calculate_jacobian(double l1, double l2, double x_offset, double theta_1, double theta_2, double theta_3) {
    Matrix<double, 3, 3> J;

    // Autogenerated using sympy codegen. 

    J(0,0) = (pow(l1, 2)*sin(theta_1)*cos(theta_1) + l1*l2*sin(theta_1)*cos(theta_1 + theta_2) + l1*l2*sin(theta_1 + theta_2)*cos(theta_1) + pow(l2, 2)*sin(theta_1 + theta_2)*cos(theta_1 + theta_2))*sin(theta_3)*cos(theta_3)/sqrt(pow(l1, 2)*pow(sin(theta_1), 2) + 2*l1*l2*sin(theta_1)*sin(theta_1 + theta_2) + pow(l2, 2)*pow(sin(theta_1 + theta_2), 2) + pow(x_offset, 2));
    J(0,1) = (l1*l2*sin(theta_1)*cos(theta_1 + theta_2) + pow(l2, 2)*sin(theta_1 + theta_2)*cos(theta_1 + theta_2))*sin(theta_3)*cos(theta_3)/sqrt(pow(l1, 2)*pow(sin(theta_1), 2) + 2*l1*l2*sin(theta_1)*sin(theta_1 + theta_2) + pow(l2, 2)*pow(sin(theta_1 + theta_2), 2) + pow(x_offset, 2));
    J(0,2) = -sqrt(pow(l1, 2)*pow(sin(theta_1), 2) + 2*l1*l2*sin(theta_1)*sin(theta_1 + theta_2) + pow(l2, 2)*pow(sin(theta_1 + theta_2), 2) + pow(x_offset, 2))*pow(sin(theta_3), 2) + sqrt(pow(l1, 2)*pow(sin(theta_1), 2) + 2*l1*l2*sin(theta_1)*sin(theta_1 + theta_2) + pow(l2, 2)*pow(sin(theta_1 + theta_2), 2) + pow(x_offset, 2))*pow(cos(theta_3), 2);
    J(1,0) = -l1*sin(theta_1) - l2*sin(theta_1 + theta_2);
    J(1,1) = -l2*sin(theta_1 + theta_2);
    J(1,2) = 0;
    J(2,0) = (pow(l1, 2)*sin(theta_1)*cos(theta_1) + l1*l2*sin(theta_1)*cos(theta_1 + theta_2) + l1*l2*sin(theta_1 + theta_2)*cos(theta_1) + pow(l2, 2)*sin(theta_1 + theta_2)*cos(theta_1 + theta_2))*sin(theta_3)/sqrt(pow(l1, 2)*pow(sin(theta_1), 2) + 2*l1*l2*sin(theta_1)*sin(theta_1 + theta_2) + pow(l2, 2)*pow(sin(theta_1 + theta_2), 2) + pow(x_offset, 2));
    J(2,1) = (l1*l2*sin(theta_1)*cos(theta_1 + theta_2) + pow(l2, 2)*sin(theta_1 + theta_2)*cos(theta_1 + theta_2))*sin(theta_3)/sqrt(pow(l1, 2)*pow(sin(theta_1), 2) + 2*l1*l2*sin(theta_1)*sin(theta_1 + theta_2) + pow(l2, 2)*pow(sin(theta_1 + theta_2), 2) + pow(x_offset, 2));
    J(2,2) = sqrt(pow(l1, 2)*pow(sin(theta_1), 2) + 2*l1*l2*sin(theta_1)*sin(theta_1 + theta_2) + pow(l2, 2)*pow(sin(theta_1 + theta_2), 2) + pow(x_offset, 2))*cos(theta_3);

    return J;
}

Vector3d calculate_force(double l1, double l2, double x_offset, double theta_1, double theta_2, double theta_3, Vector3d joint_torques) {
    Matrix<double, 3, 3> J = calculate_jacobian(l1, l2, x_offset, theta_1, theta_2, theta_3);

    // we are allowed to do inverses on square matricies (3x3 here) that are non-singular.
    // joint torques must be a 3x1 column vector 
    // f = (J^T)^-1 * T

    Vector3d force_foot = J.transpose().inverse() * joint_torques;

    return force_foot;
}

// return whether each foot is contacting, in order in the vector
std::vector<bool> detect_contact(double l1, double l2, double x_offset, vector<double> joint_positions, vector<double> joint_torques, double contact_force_threshold) {

    vector<bool> contact_vector;

    // mjbot angle measurements are in revs, must be converted to radians first

    for (int leg_number = 0; leg_number < 4; leg_number++) {
        int start_idx = leg_number*3;
        double torque_HAA = joint_torques[start_idx];
        double torque_HFE = joint_torques[start_idx + 1];
        double torque_KFE = joint_torques[start_idx + 2];
        
        double angle_HAA = revs_to_radians(joint_positions[start_idx]);
        double angle_HFE = revs_to_radians(joint_positions[start_idx + 1]);
        double angle_KFE = revs_to_radians(joint_positions[start_idx + 2]);

        Vector3d leg_joint_torques(torque_HAA, torque_HFE, torque_KFE);

        Vector3d contact_forces = calculate_force(l1, l2, x_offset, angle_HAA, angle_HFE, angle_KFE, leg_joint_torques);

        contact_vector.push_back(contact_forces.z() >= contact_force_threshold);
    }

    return contact_vector;
}
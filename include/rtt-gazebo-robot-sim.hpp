#ifndef COMAN_SIM_HPP
#define COMAN_SIM_HPP

#include <rtt/TaskContext.hpp>
#include <rtt/Port.hpp>
#include <rtt/Component.hpp>
#include <rtt/os/Semaphore.hpp>

#include <gazebo/gazebo.hh>
#include <gazebo/physics/physics.hh>
#include <gazebo/common/common.hh>

#include <Eigen/Dense>

#include <vector>

#include <Eigen/Core>
#include <time.h>
#include <rtt/os/TimeService.hpp>
#include <sstream>
#include <rtt/Logger.hpp>

#include <thread>
#include <memory>


#include <control_modes.h>
#include <kinematic_chain.h>
#include <force_torque_sensor.h>
#include <imu.h>
#include <boost/shared_ptr.hpp>

#include <srdfdom_advr/model.h>
#include <urdf/model.h>
#include <parser.h>
#include <XBotCoreModel/XBotCoreModel.h>

#include <rst-rt/geometry/Pose.hpp>
#include <rst-rt/kinematics/Twist.hpp>

#ifdef USE_INTROSPECTION
#include <rtt-core-extensions0.2/rtt-core-extensions/rtt-introspection-base.hpp>
//#include <rtt-core-extensions/rtt-introspection-base.hpp>
#include <rst-rt/monitoring/CallTraceSample.hpp>
#endif

namespace cogimon {

#ifdef USE_INTROSPECTION
class robotSim: public cogimon::RTTIntrospectionBase {
#else
class robotSim: public RTT::TaskContext {
#endif
public:
    robotSim(std::string const& name);
    #ifdef USE_INTROSPECTION
    bool configureHookInternal();
    void updateHookInternal();
	  bool startHookInternal();
	  void stopHookInternal();
	  void cleanupHookInternal();
    #else
    bool configureHook();
    void updateHook();
    void cleanupHook();
    #endif
    void WorldUpdateBegin();
    void WorldUpdateEnd();
    ~robotSim();

protected:
    bool getModel(const std::string& model_name);
    void gazeboUpdateHook(gazebo::physics::ModelPtr model);
    bool gazeboConfigureHook(gazebo::physics::ModelPtr model);
    bool setControlMode(const std::string& kinematic_chain, const std::string& controlMode);
    std::vector<std::string> getKinematiChains();
    std::string getControlMode(const std::string& kinematic_chain);
    std::vector<std::string> getControlAvailableMode(const std::string& kinematic_chain);
    std::string printKinematicChainInformation(const std::string& kinematic_chain);
    bool loadURDFAndSRDF(const std::string& URDF_path, const std::string& SRDF_path);
    std::map<std::string, std::vector<std::string> > getKinematiChainsAndJoints();
    bool resetModelConfiguration();
    bool setInitialPosition(const std::string& kin_chain, const std::vector<double>& init);
    bool runtimeVelPidUpdate(const std::string &kin_chain, const std::string& joint_name,
                           const double& p, const double& i, const double& d);
    std::vector<std::string> getForceTorqueSensorsFrames();
    bool getLinkPoseVelocityGazebo(const std::string& link_name,
                           rstrt::geometry::Pose& pose,
                           rstrt::kinematics::Twist& twist);
    std::vector<std::string> getIMUSensorsFrames();

    gazebo::physics::ModelPtr model;
    gazebo::event::ConnectionPtr world_begin;
    gazebo::event::ConnectionPtr world_end;

    RTT::SendHandle<gazebo::physics::ModelPtr(const std::string&, double)> get_model_handle;

    gazebo::physics::Joint_V gazebo_joints_;
    gazebo::physics::Link_V model_links_;

    std::map<std::string, boost::shared_ptr<KinematicChain>> kinematic_chains;
    std::vector<force_torque_sensor> force_torque_sensors;
    std::vector<imu_sensor> imu_sensors;

    bool _models_loaded;
    XBot::XBotCoreModel _xbotcore_model;

    gain_parser gains;

    #ifdef USE_INTROSPECTION
    rstrt::monitoring::CallTraceSample cts_worldUpdate;
    #endif

private:
    bool is_configured;
};

}
#endif

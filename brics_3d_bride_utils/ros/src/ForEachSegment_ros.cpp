// ROS includes
#include <ros/ros.h>
#include <dynamic_reconfigure/server.h>
#include <brics_3d_bride_utils/ForEachSegmentConfig.h>

// ROS message includes
#include <sensor_msgs/PointCloud2.h>
#include <brics_3d_msgs/PointCloudSegments.h>



#include <ForEachSegment_common.cpp>


class ForEachSegment_ros
{
	public:
		ros::NodeHandle n_;
		
		dynamic_reconfigure::Server<brics_3d_bride_utils::ForEachSegmentConfig> server;
  		dynamic_reconfigure::Server<brics_3d_bride_utils::ForEachSegmentConfig>::CallbackType f;
		

		ros::Publisher segment_;
		

		ros::Subscriber allSegments_;
        
 
        ForEachSegment_data component_data_;
        ForEachSegment_config component_config_;
        ForEachSegment_impl component_implementation_;

        ForEachSegment_ros()
        {
       	
  			f = boost::bind(&ForEachSegment_ros::configure_callback, this, _1, _2);
  			server.setCallback(f);
        	
        	
        
				segment_ = 	n_.advertise<sensor_msgs::PointCloud2>("segment", 1);
					allSegments_ =  n_.subscribe("allSegments", 1, &ForEachSegment_ros::topicCallback_allSegments, this);
  	

            
        }
        
        void topicCallback_allSegments(const brics_3d_msgs::PointCloudSegments::ConstPtr& msg)
		{
            component_data_.in_allSegments = *msg;
            update(); //call only if defined as event port, not called if it is an data port
            
        }
		
		void configure_callback(brics_3d_bride_utils::ForEachSegmentConfig &config, uint32_t level) 
		{
		}

        void configure()
        {
			component_implementation_.configure(component_config_);
        }

        void update()
        {
            component_implementation_.update(component_data_, component_config_);
            for (unsigned int i = 0; i < component_data_.in_allSegments.segments.size(); ++i) {
				segment_.publish(component_data_.in_allSegments.segments[i]);
			}
        }
 
};

int main(int argc, char** argv)
{

	ros::init(argc, argv, "ForEachSegment");

	ForEachSegment_ros node;
    node.configure();

 // if cycle time == 0 do a spin() here without calling node.update() 
	ros::spin();
	
    return 0;
}

// trumpet.cpp : Defines the entry point for the console application.
//

#include <openvr.h>
#include <vector>
#include <chrono>
#include <thread>
#include <openvr_string_std.h>

using namespace vr;
using namespace std;
using namespace openvr_string;

void UpdateLoop(COpenVRContext &context, int tracker_index)
{
	int counter = 0;
	while (1)
	{
		vr::VRControllerState_t tracker_state;
		vr::TrackedDevicePose_t pose;
		if (context.VRSystem()->GetControllerStateWithPose(TrackingUniverseStanding, tracker_index, &tracker_state, sizeof(tracker_state), &pose))
		{

			if (pose.bPoseIsValid && pose.bDeviceIsConnected)
			{
				std::string pose_string = to_string(pose);
				printf("%s\n", pose_string.c_str());
				for (int axis_index = 0; axis_index < k_unControllerStateAxisCount; axis_index++)
				{
					printf("%d: %f %f\n", counter, tracker_state.rAxis[axis_index].x, tracker_state.rAxis[axis_index].y);
				}
			}
		}

		this_thread::sleep_for(std::chrono::milliseconds(1000));
		ETrackingUniverseOrigin eOrigin = TrackingUniverseRawAndUncalibrated;
		VREvent_t event;
		if (context.VRSystem()->PollNextEventWithPose(eOrigin, &event, sizeof(event), &pose))
		{

		}

		counter++;
	}
}

void print_device_properties(vr::TrackedDeviceIndex_t index)
{
	// got a device
	vector<ETrackedDeviceProperty> keys = {
		Prop_ModelNumber_String,
		Prop_SerialNumber_String,
		Prop_RenderModelName_String,
		Prop_InputProfilePath_String,
		Prop_AttachedDeviceId_String,
		Prop_ControllerType_String,
		Prop_LegacyInputProfile_String
	};

	printf("%d: \n", index);
	for (auto key : keys)
	{
		char buf[256];
		ETrackedPropertyError prop_error;
		VRSystem()->GetStringTrackedDeviceProperty(
			index, key, buf, sizeof(buf), &prop_error);
		printf("    %s %s\n", ETrackedDevicePropertyToString(key), buf);
	}


}

int main()
{
	EVRInitError eError = VRInitError_None;
	EVRApplicationType eApplicationType = VRApplication_Other;

	IVRSystem *system = vr::VR_Init(&eError, eApplicationType);

	COpenVRContext context;
	bool found_tracker = false;
	int tracker_index;

	for (int index = 0; index < k_unMaxTrackedDeviceCount; index++)
	{
		if (context.VRSystem()->IsTrackedDeviceConnected(index))
		{
			print_device_properties(index);

			if (!found_tracker && context.VRSystem()->GetTrackedDeviceClass(index) == TrackedDeviceClass_GenericTracker)
			{
				found_tracker = true;
				tracker_index = index;
			}
		}
	}

	bool found_joystick = false;
	int joystick_axis;

	if (found_tracker)
	{
		vector<ETrackedDeviceProperty> axis_keys =
		{
			Prop_Axis0Type_Int32,
			Prop_Axis1Type_Int32,
			Prop_Axis2Type_Int32,
			Prop_Axis3Type_Int32,
			Prop_Axis4Type_Int32
		};
		for (int axis_index = 0; axis_index < axis_keys.size(); axis_index++)
		{
			int ret = context.VRSystem()->GetInt32TrackedDeviceProperty(tracker_index, axis_keys[axis_index]);
			if (ret != 0)
			{
				EVRControllerAxisType axis_type = (EVRControllerAxisType)ret;
				if (axis_type == k_eControllerAxis_Joystick)
				{
					found_joystick;
					joystick_axis = axis_index;
					break;
				}
			}
		}
	}

	// ask for tracker trigger state

	if (found_tracker) //&& found_joystick)
	{
		UpdateLoop(context, tracker_index);
	}
    return 0;
}


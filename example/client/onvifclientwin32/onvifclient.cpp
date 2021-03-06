// onvifclient.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "onvifclient.hpp"


int _tmain(int argc, _TCHAR* argv[])
{
	int ret;
	/* 192.168.1.1 is the NVT, 192.168.1.234 is the NVC */
	string url = "http://192.168.1.1/onvif/device_service";
	/* Below is where to receive the event */
	string eventNotify = "http://192.168.1.234:9090/subscription-2";
	string user = "admin";
	string pass =  "admin";
	OnvifClientDevice onvifDevice(url, user, pass);
	_tds__GetCapabilitiesResponse capabilitiesResponse;

	onvifDevice.GetCapabilities();

	OnvifClientMedia media(onvifDevice);
	_trt__GetProfilesResponse profiles;
	media.GetProfiles(profiles);
	
	/* Recording */
	OnvifClientRecording onvifRecording(onvifDevice);
	_trc__GetRecordingsResponse recordings;
	onvifRecording.GetRecordings(recordings);
	
	/* Onvif Receiver */
	OnvifClientReceiver onvifReceiver(onvifDevice);
	_trv__GetReceiversResponse receivers;
	onvifReceiver.GetReceivers(receivers);
	
	string replayUrl;
	OnvifClientReplay onvifRelay(onvifDevice);
	onvifRelay.GetReplayUri("RecordingToken001", replayUrl);
	printf("Relay Url %s\n", replayUrl.c_str());

	OnvifClientEvent onvifEvent(onvifDevice);

	onvifEvent.Subscribe(eventNotify);
	
	OnvifClientEventNotify notify(soap_new());
	
    if ((ret = soap_bind(&notify, NULL, 9090, 100) == SOAP_INVALID_SOCKET ) ) 
	{
        printf("OnvifClientEventNotify::soap_bind Binding on %d port failed", 9090);
        return 0;
    }
	int timeStart = time(NULL);
	int currentTime = 0;

	//Loop to recevie the event
	while(1)
	{
		currentTime = time(NULL);
		if (currentTime - timeStart > 58)
		{
			onvifEvent.Renew();
		}
		printf("soap_accept accepting\n");
		if( (ret = soap_accept(&notify)) == SOAP_INVALID_SOCKET) {
			printf("soap_accept accepting timeout\n");
			continue;
		}

        if ( (soap_begin_serve(&notify)) != SOAP_OK) {
            printf("soap_begin_serve serve %d failed", ret);
            continue;
        }

        ret = notify.dispatch();
        continue;
	}
	return 0;
}


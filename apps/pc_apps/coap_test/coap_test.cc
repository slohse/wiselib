
#include <iostream>
#include <iomanip>

#include "external_interface/pc/pc_os_model.h"
#include "external_interface/pc/pc_timer.h"

#include "util/pstl/static_string.h"
#include "radio/coap/coap_packet.h"
#include "radio/coap/coapradio.h"

#include "udp4radio.h"

using namespace wiselib;

typedef PCOsModel Os;
typedef UDP4Radio<Os> UPDRadio;

Os::Timer *timer;

class MyClass {
	public:
		MyClass() {
			timer = new Os::Timer();
			timer->set_timer<MyClass, &MyClass::on_time>(3000, this, (void*)timer);
		}
		
		~MyClass() {
			delete timer;
		}
		
		void on_time(void*) {
			std::cout << "It's time!" << std::endl;
			timer->set_timer<MyClass, &MyClass::on_time>(1000, this, (void*)timer);
		}
	
	private:
		Os::Timer *timer;
};


int main(int argc, char** argv) {
	
//	MyClass c;

	int port = COAP_STD_PORT;
	int coapsocket, acceptsocket, clilen, n;
	char buffer[256];
	struct sockaddr_in serv_addr, cli_addr;
	
	UDP4Radio<Os> udpradio;

	CoapPacket<Os, UDP4Radio<Os>, StaticString> testpacket;

	CoapRadio<Os, UDP4Radio<Os>, Os::Timer, Os::Debug, Os::Rand, wiselib::StaticString> cradio_;

	// Loop forever in a resource-efficient way
	// so timer events will actually occur
	while(true) pause();
	
	return 0;
}



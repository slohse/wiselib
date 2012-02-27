
#include <iostream>
#include <iomanip>

#include "external_interface/pc/pc_os_model.h"
#include "external_interface/pc/pc_timer.h"

#include "util/pstl/static_string.h"
#include "radio/coap/coap_packet.h"
#include "radio/coap/coapradio.h"

#include "udp4radio.h"
#include "ipv4_socket.h"
//#include "coapresource.h"
#include "touppercase.h"

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

	Os::Radio::self_pointer_t radio_;
	Os::Timer::self_pointer_t timer_;
	Os::Debug::self_pointer_t debug_;
	Os::Rand::self_pointer_t rand_;

	typedef wiselib::StaticString string_t;

	debug_ = new Os::Debug();
	timer_ = new Os::Timer();
	rand_ =  new Os::Rand();

	int port = COAP_STD_PORT;
	int coapsocket, acceptsocket, clilen, n;
	char buffer[256];
	struct sockaddr_in serv_addr, cli_addr;
	
	coapsocket = socket(AF_INET, SOCK_DGRAM, 0);
	if(coapsocket < 0)
	{
		cerr << "Couldn't open Socket\n";
		exit(EXIT_FAILURE);
	}

	cout << "Created Socket " << port <<"\n";

	bzero((char *) &serv_addr, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(coapsocket, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
	{
		int binderr = errno;
		cerr << "ERROR on binding, " << binderr << ": " << strerror(errno) << "\n";
		exit(EXIT_FAILURE);
	}

	cout << "Bound Socket " << port << "\n";

	listen(coapsocket,5);

	cout << "Listening " << port << "\n";

	UDP4Radio<Os> *udpradio = new UDP4Radio<Os>();

	typedef CoapRadio<Os, UDP4Radio<Os>, Os::Timer, Os::Debug, Os::Rand, string_t> coap_radio_t;

	coap_radio_t cradio_;

	cradio_.init( *udpradio,
			*timer_,
			*debug_,
			*rand_);

	cradio_.enable_radio();

	ToUpperCase<coap_radio_t> uppercaser = ToUpperCase<coap_radio_t>();
	uppercaser.init(cradio_);

	wiselib::StaticString to_upper_path = wiselib::StaticString("touppercase");

	int to_upper_id = cradio_.reg_resource_callback< ToUpperCase<coap_radio_t>, &ToUpperCase<coap_radio_t>::receive_coap >( to_upper_path, &uppercaser );

	udpradio->set_socket( coapsocket );

	// Loop forever in a resource-efficient way
	// so timer events will actually occur
	while(true)
	{
		bzero(buffer,256);
		clilen = sizeof(cli_addr);
		n = recvfrom(coapsocket, buffer, 256, 0, (struct sockaddr *) &cli_addr, (socklen_t*) &clilen);

		if (n < 0)
		{
			cerr << "ERROR reading from socket, " << "\n";
			exit(EXIT_FAILURE);
		}
		printf("received %i Bytes:\n",n);

		IPv4Socket sender( cli_addr.sin_addr.s_addr, cli_addr.sin_port );
		udpradio->notify_receivers( sender, n, (UDP4Radio<Os>::block_data_t *) buffer );
	}
	
	return 0;
}



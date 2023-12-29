#include <iostream>
#include <chrono>
#include <boost/asio.hpp>
#include <format>

using namespace std;
using namespace std::chrono;
using namespace boost::asio;
using ip::tcp;

class Message {
public:
	constexpr static size_t buffer_size = 4096ULL;
	constexpr static unsigned short port = 10000U;

	time_point<system_clock> sent;
	string message;
};

int main() {
	io_service io;
	tcp::acceptor acceptor{io, tcp::endpoint(tcp::v4(), Message::port)};

	tcp::socket socket{io};
	acceptor.accept(socket);
	cout << "Connection accepted from " << socket.remote_endpoint() << endl;


	Message message;
	while (true) {
		socket.receive(buffer(&message.sent, sizeof message.sent));

		boost::asio::streambuf buf;
		read_until(socket, buf, "\n");
		message.message = buffer_cast<const char *>(buf.data());

		time_t ct = system_clock::to_time_t(message.sent);
		tm tm_now;
		localtime_s(&tm_now, &ct);
		cout << format("[{}:{}] {}", tm_now.tm_hour, tm_now.tm_min, message.message) << endl;
	}
}
#include <chrono>
#include <format>
#include <iostream>

#include <boost/asio.hpp>

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
    tcp::socket socket{io};
    socket.connect(tcp::endpoint(ip::address_v4::from_string("127.0.0.1"), Message::port));

    Message message;
    while (true) {
        string buf = string();
        cout << "Input message >> ";
        if (getline(cin, buf); cin.eof())
            break;

        message.sent = system_clock::now();
        message.message = (buf);

        message.message += '\n';

        write(socket, buffer(&message.sent, sizeof message.sent));
        write(socket, buffer(message.message));
    }
}

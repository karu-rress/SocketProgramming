#include <iostream>
#include <boost/asio.hpp>

void handle_read(const boost::system::error_code &error, std::size_t bytes_transferred, std::shared_ptr<boost::asio::ip::tcp::socket> socket, std::vector<char> &buffer) {
    if (!error) {
        std::string received_data(buffer.data(), bytes_transferred);
        std::cout << "Received message from the other party: " << received_data << std::endl;

        // Continue listening for more data
        socket->async_read_some(boost::asio::buffer(buffer),
            std::bind(handle_read, std::placeholders::_1, std::placeholders::_2, socket, buffer));
    }
    else {
        std::cerr << "Error in reading data: " << error.message() << std::endl;
    }
}

void handle_write(const boost::system::error_code &error, std::size_t bytes_transferred, std::shared_ptr<boost::asio::ip::tcp::socket> socket, const std::string &message) {
    if (!error) {
        std::cout << "Message sent to the other party: " << message << std::endl;
    }
    else {
        std::cerr << "Error in writing data: " << error.message() << std::endl;
    }
}

void start_server(boost::asio::io_context &io_context, unsigned short port) {
    using namespace boost::asio;
    using ip::tcp;

    // Create an acceptor to listen on the specified port
    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), port));

    // Create a socket
    auto socket = std::make_shared<tcp::socket>(io_context);

    // Listen for incoming connections
    acceptor.async_accept(*socket, [socket](const boost::system::error_code &error) {
        if (!error) {
            std::cout << "Client connected." << std::endl;

            // Create a buffer to store received data
            std::vector<char> buffer(1024);

            // Start reading data from the client
            socket->async_read_some(boost::asio::buffer(buffer),
                std::bind(handle_read, std::placeholders::_1, std::placeholders::_2, socket, buffer));
        }
        else {
            std::cerr << "Error in accepting connection: " << error.message() << std::endl;
        }
        });

    // Run the IO service
    io_context.run();
}

void start_client(boost::asio::io_context &io_context, const std::string &server_address, unsigned short port) {
    using namespace boost::asio;
    using ip::tcp;

    // Create a socket
    tcp::socket socket(io_context);

    // Connect to the server
    socket.connect(tcp::endpoint(ip::address::from_string(server_address), port));
    std::cout << "Connected to the server." << std::endl;

    // Read user input and send messages to the server
    while (true) {
        std::string message;
        std::cout << "Enter message to send (or 'exit' to quit): ";
        std::getline(std::cin, message);

        if (message == "exit") {
            break;
        }

        // Send the message to the server
        async_write(socket, buffer(message),
            std::bind(handle_write, std::placeholders::_1, std::placeholders::_2, std::make_shared<tcp::socket>(std::move(socket)), message));
    }
}

int main() {
    try {
        boost::asio::io_context io_context;
        unsigned short port = 8080;

        // Start the server in a separate thread
        std::thread server_thread([&io_context, port]() {
            start_server(io_context, port);
            });

        // Start the client
        start_client(io_context, "127.0.0.1", port);

        // Join the server thread
        server_thread.join();
    }
    catch (const std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}

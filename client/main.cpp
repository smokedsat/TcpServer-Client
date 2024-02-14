#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QTcpServer>
#include <queue>
#include <thread>
#include <mutex>

class Client
{
public:
    void start()
    {
        if(connect())
        {
            std::thread read_thr([this](){
                this->read();
            });

            std::thread write_thr([this](){
                this->write();
            });

            while(write_thr.joinable() && read_thr.joinable())
            {
                std::thread get_thr([this](){
                    this->getData();
                });

                get_thr.join();
            }

            write_thr.join();
            read_thr.join();

        }
        else
        {
            qDebug() << "End";
        }
    }

    bool connect()
    {
        qDebug() << "Connecting to host";
        client.connectToHost(host, port);

        if(client.waitForConnected(1500))
        {
            qDebug() << "Successfully connected to host";
            return true;
        }
        else
        {
            qDebug() << "Error in connection to host";
            return false;
        }
    }


    void read()
    {
        qDebug() << "Reading";
        while(true)
        {
            std::this_thread::sleep_for(std::chrono::seconds(5));
            if(client.isReadable())
            {
                while(client.isReadable())
                {
                    std::this_thread::sleep_for(std::chrono::seconds(3));
                    qDebug() << "Client readable.";
                    auto bytes = client.readAll();
                    rw_mutex.lock();
                    if(bytes.size() != 0)
                    {
                        qDebug() << "Readed data size = " << bytes.size();
                        data.push(bytes);
                    }
                    rw_mutex.unlock();
                }
            }
        }
    }

    void write()
    {
        while(true)
        {
            std::this_thread::sleep_for(std::chrono::seconds(5));
            if(client.isWritable())
            {
                qDebug() << "Client writable.";
                while(client.isWritable())
                {
                    std::this_thread::sleep_for(std::chrono::seconds(3));
                    client.write("Hello...");
                }
            }
        }
    }

    void getData()
    {
        std::this_thread::sleep_for(std::chrono::seconds(7));
        qDebug() << "Getting data from queue.";
        if(!data.empty())
        {
            rw_mutex.lock();
            while(!data.empty())
            {
                qDebug() << "Data: " << data.front();
                data.pop();
            }
            rw_mutex.unlock();
        }
    }

private:
    QTcpSocket client;
    QHostAddress host = (QHostAddress)"127.0.0.1";
    quint16 port = 44333;

    std::queue<QByteArray> data;
    std::mutex rw_mutex;
};

int main(int argc, char *argv[])
{
    Client client;
    client.start();

    return 0;
}

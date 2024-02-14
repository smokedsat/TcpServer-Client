#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QTcpServer>
#include <QVector>
#include <queue>
#include <mutex>
#include <thread>


class TcpServer
{
public:

    void start()
    {
        listen();
        WORK();
    }

    void listen()
    {
        if(server.listen(host, port))
        {
            qDebug() << "Server is listening";

            std::thread listening([this](){
                this->acceptNewConnections();
                });

            listening.detach();
        }
    }

    void acceptNewConnections()
    {
        qDebug() << "Server is accepting new connections...";
        QTcpSocket * client = nullptr;

        while(server.isListening())
        {
            qDebug() << "Server still listening...";
            std::this_thread::sleep_for(std::chrono::seconds(5));

            if(server.hasPendingConnections())
            {
                qDebug() << "Server has pending connictions!";
                client = server.nextPendingConnection();

                if(client != nullptr)
                {
                    qDebug() << "New client!";
                    connections.push_back(client);
                    client = nullptr;
                }
            }
        }
        qDebug() << "Server ended accepting connections.";
    }

    void WORK()
    {

        std::thread write_thr([this](){
            this->write();
        });

        std::thread read_thr([this](){
            this->read();
        });

        while(read_thr.joinable() && write_thr.joinable())
        {
            std::thread getFromQueue([this](){
                this->getFromQueue();
            });

            if(getFromQueue.joinable())
            {
                getFromQueue.join();
            }
        }
    }

    void write()
    {
        while(true)
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            while(!connections.empty())
            {
                qDebug() << "Write: Connections aren't empty!";
                std::this_thread::sleep_for(std::chrono::seconds(5));

                for(auto & connection : connections)
                {
                    if(connection->isWritable())
                    {
                        if(connection->write("Hello from server") == -1)
                        {
                            qDebug() << "Bytes weren't written to client";
                        }
                        else
                        {
                            qDebug() << "Something was sent to client";
                        }
                    }
                }
            }
        }
    }

    void read()
    {
        while(true)
        {
            std::this_thread::sleep_for(std::chrono::seconds(2));
            if(!connections.empty())
            {
                qDebug() << "Read: Connections aren't empty!";
                while(!connections.empty())
                {
                    std::this_thread::sleep_for(std::chrono::seconds(5));
                    for(auto & connection : connections)
                    {
                        if(connection->isReadable())
                        {
                            auto data = connection->readAll();
                            insertInQueue(connection, &data);
                        }
                    }
                }
            }
        }
    }

    void insertInQueue(QTcpSocket * socket, QByteArray * data)
    {
        rw_mutex.lock();
        queue.push(qMakePair<QObject*,QByteArray*>(qMove(socket),qMove(data)));
        rw_mutex.unlock();
    }

    void getFromQueue()
    {
        rw_mutex.lock();
        while(!queue.empty())
        {
            qDebug() << queue.front().first << ": " << queue.front().second;
            queue.pop();
        }
        rw_mutex.unlock();
    }

private:
    QTcpServer server;
    QHostAddress host = (QHostAddress)"127.0.0.1";
    QVector<QTcpSocket*> connections;
    quint16 port = 44333;

    std::queue<std::pair<QObject*, QByteArray*>> queue;

    std::mutex rw_mutex;
};

int main(int argc, char *argv[])
{
    TcpServer server;
    server.start();

}

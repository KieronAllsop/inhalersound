// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#ifndef QT_GUI_LOGIN_HPP_INCLUDED
#define QT_GUI_LOGIN_HPP_INCLUDED
// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I
// Standard Library Includes
#include <memory>
#include <chrono>
#include <thread>

// Asio Includes
#include <asio.hpp>
#include <asio/high_resolution_timer.hpp>

// Qt Includes
#include <QDialog>
#include <QEvent>

// Inhaler Includes
#include "inhaler/server.hpp"

// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I


// Forward Declarations
class QLabel;
class QLineEdit;
class QPushButton;
class QWidget;


// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
namespace qt_gui {
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n


class connection_status_event : public QEvent
{
public:

    using shared_schema_t = inhaler::server::shared_schema_t;

    static QEvent::Type type()
    {
        static auto Type = QEvent::Type( QEvent::registerEventType() );
        return Type;
    }

public:

    connection_status_event
        (   const shared_schema_t& Schema,
            bool Connected,
            bool Retrying,
            int Attempt  )
    : QEvent( type() )
    , Schema_   ( Schema )
    , Connected_( Connected )
    , Retrying_ ( Retrying )
    , Attempt_  ( Attempt )
    {
    }

    const shared_schema_t& schema() const
    {
        return Schema_;
    }

    bool connected() const
    {
        return Connected_;
    }

    bool retrying() const
    {
        return Retrying_;
    }

    int attempt() const
    {
        return Attempt_;
    }

private:

    shared_schema_t Schema_;
    bool            Connected_;
    bool            Retrying_;
    int             Attempt_;

};


class login_dialog : public QDialog
{
    Q_OBJECT

public:

    using shared_server_t = std::shared_ptr<inhaler::server>;
    using shared_schema_t = inhaler::server::shared_schema_t;

    explicit                login_dialog                (   const shared_server_t& Server,
                                                            QWidget* Parent = 0   );

                            ~login_dialog               ();

    virtual bool            event                       (   QEvent* Event   );

    void                    initialise_connection       ();

private:

    using timer_t           = asio::basic_waitable_timer<std::chrono::high_resolution_clock>;
    using shared_timer_t    = std::shared_ptr<timer_t>;

signals:

    void                    change_stacked_layout_index ();

private:

    void                    on_connection_status        (   const shared_schema_t&  Schema,
                                                            bool                    Connected,
                                                            bool                    Retrying,
                                                            int                     Attempt   );


    bool                    connect_to_server           (   bool Retry,
                                                            int  Attempt   );


    void                    handle_wait                 (   const asio::error_code&     Error,
                                                            int                         Count,
                                                            int                         MaxRetries,
                                                            std::chrono::microseconds   Interval,
                                                            shared_timer_t&             Timer   );

    void                    update_login_state          ();

private slots:

    void                    on_login_clicked            ();
    void                    on_credentials_changed      (   const QString& Text   );

private:

    shared_server_t     Server_;
    shared_schema_t     Schema_;
    asio::io_context    IoContext_;
    std::thread         RetryThread_;
    bool                Connected_;

    // Owned Widgets
    QLabel*             Title_Label_;
    QLabel*             Status_Label_;
    QLineEdit*          Username_Edit_;
    QLineEdit*          Password_Edit_;
    QPushButton*        Login_Button_;

};

// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n
} // end qt_gui
// n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n n

// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#endif // QT_GUI_LOGIN_HPP_INCLUDED

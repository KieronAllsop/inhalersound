// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#ifndef QT_WINDOWS_LOGIN_HPP_INCLUDED
#define QT_WINDOWS_LOGIN_HPP_INCLUDED
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

// Custom Includes
#include "qt_windows/patientdetails.h"
#include "qt_windows/administration.h"

// Inhaler Includes
#include "inhaler/server.hpp"


// I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I I


// Forward Declarations
class QLabel;
class QLineEdit;
class QPushButton;
class QWidget;



class Login : public QDialog
{
    Q_OBJECT

public:

    using shared_server_t = std::shared_ptr<inhaler::server>;
    using shared_schema_t = inhaler::server::shared_schema_t;

    explicit            Login                   (   const shared_server_t& Server,
                                                    QWidget* Parent = 0   );

                        ~Login                  ();


    void                initialise_connection   ();

private:

    using timer_t           = asio::basic_waitable_timer<std::chrono::high_resolution_clock>;
    using shared_timer_t    = std::shared_ptr<timer_t>;

private:

    bool                connect_to_server       (   bool Retry,
                                                    int Attempt   );


    void                handle_wait             (   const asio::error_code&     Error,
                                                    int                         Count,
                                                    int                         MaxRetries,
                                                    std::chrono::microseconds   Interval,
                                                    shared_timer_t&             Timer   );

    void                update_login_state      ();

private slots:

    void                on_login_clicked        ();
    void                on_credentials_changed  (   const QString& Text   );

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

// G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G G
#endif // QT_WINDOWS_LOGIN_HPP_INCLUDED

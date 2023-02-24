#pragma once
#include <receiver.hpp>
#include <dispatcher.hpp>
#include <sender.hpp>
#include <atm_messages.hpp>

#include <iostream>
#include <thread>
#include <mutex> 

class interface_machine
{
    std::mutex iom;
    messaging::Receiver incoming;
public:
    void done()
    {
        get_sender().send(messaging::CloseQueue());
    }
    void run()
    {
        try
        {
            for(;;)
            {
                incoming.wait()
                .handle<issue_money>(
                [&](issue_money const& msg)
                {
                    {
                    std::lock_guard<std::mutex> lk(iom);
                    std::cout<<"Issuing "
                    <<msg.amount<<std::endl;
                    }
                }
                )
                .handle<display_insufficient_funds>(
                [&](display_insufficient_funds const& msg)
                {
                    {
                    std::lock_guard<std::mutex> lk(iom);
                    std::cout<<"Insufficient funds"<<std::endl;
                    }
                }
                )
                .handle<display_enter_pin>(
                [&](display_enter_pin const& msg)
                {
                    {
                    std::lock_guard<std::mutex> lk(iom);
                    std::cout
                    <<"Please enter your PIN (0-9)"
                    <<std::endl;
                    }
                }
                )
                .handle<display_enter_card>(
                [&](display_enter_card const& msg)
                {
                    {
                    std::lock_guard<std::mutex> lk(iom);
                    std::cout<<"Please enter your card (I)"
                    <<std::endl;
                    }
                }
                )
                .handle<display_balance>(
                [&](display_balance const& msg)
                {
                    {
                    std::lock_guard<std::mutex> lk(iom);
                    std::cout
                    <<"The balance of your account is "
                    <<msg.amount<<std::endl;
                    }
                }
                )       
                .handle<display_withdrawal_options>(
                [&](display_withdrawal_options const& msg)
                {
                    {
                    std::lock_guard<std::mutex> lk(iom);
                    std::cout<<"Withdraw 50? (w)"<<std::endl;
                    std::cout<<"Display Balance? (b)"
                    <<std::endl;
                    std::cout<<"Cancel? (c)"<<std::endl;
                    }
                }
                )
                .handle<display_withdrawal_cancelled>(
                [&](display_withdrawal_cancelled const& msg)
                {
                    {
                    std::lock_guard<std::mutex> lk(iom);
                    std::cout<<"Withdrawal cancelled"
                    <<std::endl;
                    }
                }
                )
                .handle<display_pin_incorrect_message>(
                [&](display_pin_incorrect_message const& msg)
                {
                    {
                    std::lock_guard<std::mutex> lk(iom);
                    std::cout<<"PIN incorrect"<<std::endl;
                    }
                }
                )
                .handle<eject_card>(
                [&](eject_card const& msg)
                {
                    {
                    std::lock_guard<std::mutex> lk(iom);
                    std::cout<<"Ejecting card"<<std::endl;
                    }
                }
                );
            }
        }
        catch(messaging::CloseQueue&)
        {
        }
    }
    messaging::Sender get_sender()
    {
        return incoming;
    }
};
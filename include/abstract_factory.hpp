/**
 * @file abstract_factory.hpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-03-05
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#pragma once

#include "protocol.hpp"

namespace AbstractFactory
{
    class Button
    {

    };

    class MacButton : public Button
    {

    };

    class WinButton : public Button 
    {

    };

    class Border
    {

    };

    class MacBorder : public Border 
    {

    };

    class WinBorder : public Border 
    {

    };

    class AbstractFactory 
    {
    public:
        virtual Button* CreateButton() = 0;
        virtual Border* CreateBorder() = 0;
    };

    class MacFactory : public AbstractFactory
    {
    public:
        MacButton* CreateButton()
        {
            return new MacButton;
        }

        MacBorder* CreateBorder()
        {
            return new MacBorder;
        }
    };

    class WinFactory : public AbstractFactory
    {
    public:
        WinButton* CreateButton()
        {
            return new WinButton;
        }

        WinBorder* CreateBorder()
        {
            return new WinBorder;
        }
    };
}  // namespace AbstractFactory
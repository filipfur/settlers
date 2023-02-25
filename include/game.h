#pragma once

class Game
{
public:
    enum class State
    {
        Init,
        Player_Select_Start,
        Waiting_For_Player,
        Waiting_For_Engine
    };

    Game(int numberOfPlayers)
    : _numberOfPlayers{numberOfPlayers}
    {

    }

    virtual ~Game() noexcept
    {

    }

    void start()
    {
        _state = State::Player_Select_Start;
        _currentPlayer = 0;
    }


    void playerSelectedStart()
    {
        if(_currentPlayer < _numberOfPlayers - 1)
        {
            ++_currentPlayer;
        }
        else
        {
            _state = State::Waiting_For_Player;
            _currentPlayer = 0;
        }
    }

    void turnPlayed()
    {
        _state = State::Waiting_For_Engine;
    }

    void newTurn()
    {
        _state = State::Waiting_For_Player;
        if(_currentPlayer < _numberOfPlayers - 1)
        {
            ++_currentPlayer;
        }
        else
        {
            _currentPlayer = 0;
        }
    }


    int currentPlayer() const
    {
        return _currentPlayer;
    }

    State state() const
    {
        return _state;
    }

private:
    int _numberOfPlayers;
    State _state{State::Init};
    int _currentPlayer{0};
};
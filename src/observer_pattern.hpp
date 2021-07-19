//
// Created by yichen on 2021-02-07.
//
#pragma once

#include <vector>
#include <functional>

#include <tiny_ecs.hpp>

using Callback_entity = std::function<void(ECS::Entity &)>;
using Callback = std::function<void()>;

class Subject
{
    std::vector<Callback_entity> observers_entity;
    std::vector<Callback> observers;

public:
    void add_observer_entity(Callback_entity cb)
    {
        observers_entity.push_back(cb);
    }

    void add_observer(Callback cb)
    {
        observers.push_back(cb);
    }

    //void removeObserver(Callback cb)
    //{
    //    observers.erase(std::remove(observers.begin(), observers.end(), cb), observers.end()); // cannot remove by value, combine std::remove with vector::erase
    //}
    
    void notify_all_entity(ECS::Entity & entity)
    {
        for (Callback_entity cb : observers_entity)
        {
            cb(entity);
        }
    }

    void notify_all()
    {
        for (Callback cb : observers)
        {
            cb();
        }
    }
};

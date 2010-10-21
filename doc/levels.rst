.. _levels:

JSON series format
==================

.. highlight:: c

This is the documentation for the level serie format used in Zoob_ |release|, last updated |today|.

Main components
---------------
Serie
*****

A serie is simply a collection of levels that should be played one after the other by the player. One serie is represented as a single JSON object.
It must contains at least the following properties :

- *levels*
- *name*

Level
*****
A level can be see as a JSON object with the following fields :

- *tanks*
- optional *shadows*
- optional *boss*
- *ydim*
- *xdim*
- *tiles*

Tank
****
A tank contains the following fields :

- *type*
- *coords*

Level tile
**********
A level tile is either a single string specifying the tile type or an object with a *type* property and some other advanced properties such as :

- *brk* : indicate if this tile is breakable
- *life* : how many rockets are needed to destroy this tile (default = 1)
- *trigger*

Triggers
--------

Trigger can be specified on a per-tile basis. They are executed when a specific *condition* is met. One tile might have multiple filters.

.. code-block:: js

  {
    "condition": "destroy"
    "code": ""
    "effect" : {"off":"none", "on":"glow"}
  }

The *code* field of a trigger contains a bit of code that will be executed when the condition is met.

The *effect* field can be used to specify a visual effect that should be displayed on the tile containing this trigger when this trigger is off and when it is triggered.

Conditions
**********

The available conditions are :

- **every x** executed every x seconds (x is a float, >= 1)
- **destroy** 
- **enemy_in**
- **player_in**
- **tank_in**
- **enemy_rocket_in**
- **player_rocket_in**
- **rocket_in**

Multiple conditions might be combined by using *&&* and *||*.

Trigger code
************
Please not that if the trigger action is *impossible* from the game physics point of view (like teleporting a tank to a solid tile), the action will simply be canceled and the trigger won't be notified about it.

Available API
*************
Trigger code should have access to the following api :

- spawn tank
- teleport tank
- destroy rockets
- redirect rockets
- check for any tile status
- access neighbouring tile

Ideas
*****

Trigger that spawns a tank when the tile is destroyed :

.. code-block:: js
  
  {
    "condition": "destroy"
    "code" : "spawn_tank(get_this(), simple_boss);"
  }

An trigger that teleports the player to another tile :

.. code-block:: js

  {
    "condition": "teleport"
    "code" : "teleport(get_player(), (2,2));"
  }

A trigger spawning one tank per second on empty neighbouring tiles. It stops when its tile is not solid anymore.

.. code-block:: js
  
  {
    "condition": "every 4"
    "code" : "if (!get_this().solid())
                return;

              foreach (get_neighbours() as neigh) {
                if (spawn_tank(neigh, simple_tank))
                  break;
              }"
  }



.. _levels:

Zoob networking
==================

.. highlight:: c

This is the documentation for the networking model used in Zoob_ |release|, last updated |today|.

Main components
---------------
Zoob uses a client server model. Both the client and the server are run in a thread separate from the main thread that is handling game logic and rendering. 

NetController
*************
The NetController is an interface that is implemented both by Client and Server. It provides the game logic with facility to query the state of networked entities (tanks, rockets, mines). It also allow the logic to send a player command struct describing the action takent by the local player. 
In the client implementation of NetController, the new commands will be sent to the server and the states queried from the server. In the server implementation, a new command will be broadcasted to client and the state is known locally (since the server maintains the state of the whole game).

In the current implementation, the server has full authority over the game logic. This is somehow unsafe because the server might be run by an untrusted party (a player's phone), but Zoob is still a small game so we don't expect many cheaters. Some anti-cheat methods might be implemented later. One such method would allow client that suspect a user of cheating (using a modified server) to report it to a central database. If a user is reporter multiple time, he would be marked as a cheater.


Network states
--------------
The server can be in three main states. All the network gamemodes have the concept of "rounds". At the beginning of each round, spectators might enter the game and start playing. These states are very similar to what is used in Quake3 Arena.

Warm-up
*******
Before the first round of the game start. Players might spawn and frag each other freely.

Round-start
***********
Usually lasts from 5 to 10 seconds. Allow spectator to join and spawn.

In-Round
********
Player are currently playing a round, new player have to wait for the round to end

Game finished
*************
The game has ended. After some time, the server will start a new one


Network messages
----------------
When a new client connects to the server, it will first request the current level. The level is sent in a zipped json file. Yeah, the level is sent every time, but levels are REALLY small. (less than 1k when zipped).

After the client has received the level, the server will start to send the client updates. Most of the update are gamestate updates (basically sending the entities positions and velocity). End-of-round and End-of-game updates are also possible. Finally, when changing level, the server will also send an update to the client.

Players are uniquely identified by an ID that is delivered by the server when the client first connects. 
This is the same for other entities.

Below, message are marked with a (r) when they should be reliable. The #x before a message block indicates the order. If only # is present, this means that the message block have no order.

Client is CONNECTING
****************

Client is CONNECTING 
Client          Server
#1
  -> HELLO (r)
#2
  <- CURRENT_LEVEL + CURRENT_STATE (warmup, roundstart, inround) (r)
  <- KICKED (r)
#3 if serverstate == warmup || serverstate == roundstart. Otherwise, go in spectating client state
  -> JOIN (r)
  <- JOINED (r)
  <- NOT_JOINED (in case the round started before) (r)

Client is PLAYING/SPECTATING
****************************
#
  -> PLAYER_COMMANDS (r)
#
(important remark regarding gamestate : we distinguish two types of gamestate informations. Some are important
and should therefore use reliable packets while other a not so important and can safely be discarded if a more
up to date similar command arrives. We use libenet features for sequencing and reliability)
  <- IMPORTANT GAMESTATE (spawn, explosions, damage, death, ...) (r)
  <- SEQUENCED UNRELIABLE GAMESTATE (players positions, velocities) 

  <- SERVER_STATE (WARMUP, ROUNDSTART, INROUND, GAME_FINISHED)  (r)
  <- CURRENT_LEVEL + CURRENT_STATE (warmup, roundstart, inround) (same as welcome message in CONNECTING)  (r)



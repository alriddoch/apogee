// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Aloril and Al Riddoch

#ifndef NET_CONNECTION_H
#define NET_CONNECTION_H

#include <Atlas/Objects/Decoder.h>
#include <Atlas/Objects/Encoder.h>
#include <Atlas/Codec.h>

#include <string>
#include <skstream.h>
#include <stdio.h>

class World;
class Connection;

class SightDecoder : public Atlas::Objects::Decoder
{
  protected:
    void UnknownObjectArrived(const Atlas::Message::Object&);
    void ObjectArrived(const Atlas::Objects::Operation::Create&);
    void ObjectArrived(const Atlas::Objects::Operation::Move&);
    void ObjectArrived(const Atlas::Objects::Operation::Set&);

    Connection & m_connection;

  public:
    SightDecoder(Connection & c) : m_connection(c) { }

    void processSight(const Atlas::Message::Object&);
};

class Connection : public Atlas::Objects::Decoder
{
  private:
    bool error_flag;
    bool reply_flag;
    int cli_fd;
    Atlas::Objects::Encoder * encoder;
    Atlas::Codec<std::iostream> * codec;
    client_socket_stream ios;
    std::string account_id;
    std::string character_id;
    enum {
        INIT,
        LOGGED_IN,
        CREATED_CHAR
    };
    int state;
    SightDecoder sdecode;
    World & m_world;
  protected:
    void UnknownObjectArrived(const Atlas::Message::Object&);
    void ObjectArrived(const Atlas::Objects::Operation::Info&);
    void ObjectArrived(const Atlas::Objects::Operation::Error&);
    void ObjectArrived(const Atlas::Objects::Operation::Sight&);
  public:
    Connection(World & world);
     
    void send(Atlas::Objects::Operation::RootOperation *);
    bool connect(const std::string & server);
    void login(const std::string & user, const std::string & password);
    void create_char(const std::string & name, const std::string & type);
    void look();
    void move();
    bool poll();

    World & getWorld() const { return m_world; }
};

#endif

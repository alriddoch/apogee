// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2000-2001 Aloril and Al Riddoch

#include <fstream.h>

extern "C" {
    #include <stdio.h>
    #include <stdlib.h>
    #include <sys/time.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <unistd.h>
}

// iosockinet - the iostream-based socket class
// Atlas negotiation
#include <Atlas/Codec.h>
#include <Atlas/Net/Stream.h>
#include <Atlas/Objects/Root.h>
#include <Atlas/Objects/Encoder.h>
#include <Atlas/Objects/Entity/Account.h>
#include <Atlas/Objects/Entity/GameEntity.h>
#include <Atlas/Objects/Operation/Login.h>
// The DebugBridge
#include "Connection.h"

#include <world/World.h>

using Atlas::Objects::Entity::Account;
using Atlas::Objects::Entity::GameEntity;

using Atlas::Objects::Operation::RootOperation;
using Atlas::Objects::Operation::Create;
using Atlas::Objects::Operation::Error;
using Atlas::Objects::Operation::Info;
using Atlas::Objects::Operation::Login;
using Atlas::Objects::Operation::Look;
using Atlas::Objects::Operation::Move;
using Atlas::Objects::Operation::Set;
using Atlas::Objects::Operation::Sight;

using Atlas::Message::Object;

Connection::Connection(World & world) : error_flag(false), reply_flag(false),
                           cli_fd(-1), encoder(NULL), codec(NULL),
                           ios(client_socket_stream::TCP), state(INIT),
                           sdecode(*this), m_world(world)
{
}

void Connection::send(RootOperation * op)
{
    encoder->StreamMessage(op);
    ios << std::flush;
}

void Connection::login(const std::string& user, const std::string& password)
{
    account_id = user;
    Account account = Account::Instantiate();
    Login l = Login::Instantiate();
    error_flag = false;
    reply_flag = false;

    account.SetId(user);
    account.SetPassword(password);

    Object::ListType args(1,account.AsObject());

    l.SetArgs(args);

    encoder->StreamMessage(&l);

    ios << std::flush;

    std::cout << "logging in" << std::endl << std::flush;
    while (!reply_flag) {
        codec->Poll();
    }
    if (!error_flag) {
        std::cout << "login was a success" << std::endl << std::flush;
        return;
    }
    std::cout << "login failed" << std::endl << std::flush;

    Create c = Create::Instantiate();
    error_flag = false;
    reply_flag = false;

    c.SetArgs(args);

    encoder->StreamMessage(&c);

    ios << std::flush;

    while (!reply_flag) {
        codec->Poll();
    }

    if (!error_flag) {
        std::cout << "create account was a success" << std::endl << std::flush;
        return;
    }
}

bool Connection::connect(const std::string & server)
{

    std::cout << "Connecting to cyphesis.." << std::endl << std::flush;

    ios.open(server.c_str(), 6767);

    if (!ios.is_open()) {
        return false;
    }

    cli_fd = ios.getSocket();
    std::cout << "Connected to cyphesis" << std::endl << std::flush;
    // Connect to the server

    // Do client negotiation with the server
    Atlas::Net::StreamConnect conn("cyphesis_client", ios, this);

    std::cout << "Negotiating... " << std::flush;
    // conn.Poll() does all the negotiation
    while (conn.GetState() == Atlas::Negotiate<iostream>::IN_PROGRESS) {
        conn.Poll();
    }
    std::cout << "done" << std::endl;

    // Check whether negotiation was successful
    if (conn.GetState() == Atlas::Negotiate<iostream>::FAILED) {
        std::cerr << "Failed to negotiate" << std::endl;
        ios.close();
        return false;
    }
    // Negotiation was successful

    // Get the codec that negotiation established
    codec = conn.GetCodec();

    // This should always be sent at the beginning of a session

    encoder = new Atlas::Objects::Encoder(codec);

    codec->StreamBegin();
    return true;

}

void Connection::create_char(const std::string & name, const std::string & type)
{
    GameEntity character = GameEntity::Instantiate();
    Create c = Create::Instantiate();
    error_flag = false;
    reply_flag = false;

    Object::ListType parlist(1,type);
    character.SetParents(parlist);
    character.SetName(name);

    Object::ListType args(1,character.AsObject());

    c.SetArgs(args);
    c.SetFrom(account_id);
   
    encoder->StreamMessage(&c);
    ios << std::flush;
    while (!reply_flag) {
        codec->Poll();
    }
    if (!error_flag) {
        std::cout << "Created character: "<< character_id << " :using account: "
                  << account_id << " :" << std::endl << std::flush;
    }
}

void Connection::look()
{
    Look l = Look::Instantiate();
    l.SetFrom(character_id);

    encoder->StreamMessage(&l);
    ios << std::flush;
}

void Connection::move()
{
    Object::MapType ent;
    ent["id"] = character_id;
    Object::ListType coords;
    coords.push_back(100.0);
    coords.push_back(100.0);
    coords.push_back(0.0);
    ent["pos"] = coords;
    // ent["loc"] = std::string("world_0");
    Move m = Move::Instantiate();
    m.SetFrom(character_id);

    m.SetArgs(Object::ListType(1,ent));

    encoder->StreamMessage(&m);
    ios << std::flush;
}

bool Connection::poll()
{
    fd_set infds;
    struct timeval tv;

    FD_ZERO(&infds);

    FD_SET(cli_fd, &infds);

    tv.tv_sec = 0;
    tv.tv_usec = 0;

    int retval = select(cli_fd+1, &infds, NULL, NULL, &tv);

    if (retval) {
        if (FD_ISSET(cli_fd, &infds)) {
            if (ios.peek() == -1) {
                std::cerr << "Server disconnected" << std::endl << std::flush;
                return false;
            }
            codec->Poll();
            return true;
        }
    }
    return false;
}

void Connection::UnknownObjectArrived(const Object& o)
{
#if 0 
    std::cout << "An unknown has arrived." << std::endl << std::flush;
    if (o.IsMap()) {
        for(Object::MapType::const_iterator I = o.AsMap().begin();
		I != o.AsMap().end();
		I++) {
		std::cout << I->first << std::endl << std::flush;
                if (I->second.IsString()) {
		    std::cout << I->second.AsString()<< std::endl << std::flush;
                }
	}
    } else {
        std::cout << "Its not a map." << std::endl << std::flush;
    }
#endif
}

void Connection::ObjectArrived(const Info& o)
{
    reply_flag = true;
    std::cout << "An info operation arrived." << std::endl << std::flush;
    const Object::ListType & args = o.GetArgs();
    if (state == CREATED_CHAR) {
        return;
    }
    if (state == INIT) {
        const Object & account = args.front();
        const Object::MapType & ac_map = account.AsMap();
        Object::MapType::const_iterator I = ac_map.find("id");
        if ((I != ac_map.end()) && I->second.IsString()) {
            account_id = I->second.AsString();
        }
            
        state = LOGGED_IN;
        if (ac_map.find("characters") != ac_map.end()) {
            const Object::ListType & chars = ac_map.find("characters")->second.AsList();
            std::cout << "Got " << chars.size() << " characters:"
                      << std::endl << std::flush;
            for(Object::ListType::const_iterator I = chars.begin();
                        I != chars.end(); I++) {
                std::cout << "Character " << I->AsString()
                          << std::endl << std::flush;
            }
        }
    } else if (state == LOGGED_IN) {
        const Object::MapType & character = args.front().AsMap();
        character_id = character.find("id")->second.AsString();
        std::cout << "got char [" << character_id << "]"
                  << std::endl << std::flush;
        state = CREATED_CHAR;
    }
}

void Connection::ObjectArrived(const Error& o)
{
    reply_flag = true;
    error_flag = true;
    std::cout << "An error operation arrived." << std::endl << std::flush;
    Object::ListType args = o.GetArgs();
    Object & arg = args.front();
    if (arg.IsString()) {
        std::cout << arg.AsString() << std::endl << std::flush;
    } else {
	std::cout << arg.GetType() << std::endl << std::flush;
    } 
}

void Connection::ObjectArrived(const Sight& o)
{
    std::cout << "An sight operation arrived." << std::endl << std::flush;
    const Object::ListType & args = o.GetArgs();
    const Object & objseen = args.front();
    sdecode.processSight(objseen);
    const std::list<std::string> & ids = m_world.getIds();
    std::list<std::string>::const_iterator I;

    Look l(Look::Instantiate());
    
    for (I = ids.begin(); I != ids.end(); I++) {
        std::cout << "Sending look to " << *I << std::endl << std::flush;
        l.SetTo(*I);
        l.SetFrom(character_id);
        Object::MapType ent;
        ent["id"] = *I;
        l.SetArgs(Object::ListType(1,ent));
        send(&l);
    }
    m_world.clearLookIds();
}

void SightDecoder::processSight(const Object& o)
{
    Atlas::Objects::Decoder::ObjectArrived(o);
}

void SightDecoder::UnknownObjectArrived(const Object& o)
{
    std::cout << "An sight of an unknown operation arrived."
              << std::endl << std::flush;
    if (!o.IsMap()) { return; }
    const Object::MapType & ent = o.AsMap();

    World & w = m_connection.getWorld();
    w.updateObject(ent);

}

void SightDecoder::ObjectArrived(const Create& op)
{
    std::cout << "An sight of an create operation arrived."
              << std::endl << std::flush;
    const Object::ListType & args = op.GetArgs();
    if (args.size() != 1) {
        std::cerr << "Funny lengthed args in create op"
                  << std::endl << std::flush;
        return;
    }
    const Object & o = args.front();

    if (!o.IsMap()) { return; }
    const Object::MapType & ent = o.AsMap();

    World & w = m_connection.getWorld();
    w.updateObject(ent);

}

void SightDecoder::ObjectArrived(const Move& op)
{
    std::cout << "An sight of an move operation arrived."
              << std::endl << std::flush;
    const Object::ListType & args = op.GetArgs();
    if (args.size() != 1) {
        std::cerr << "Funny lengthed args in create op"
                  << std::endl << std::flush;
        return;
    }
    const Object & o = args.front();

    if (!o.IsMap()) { return; }
    const Object::MapType & ent = o.AsMap();

    World & w = m_connection.getWorld();
    w.updateObject(ent);

}

void SightDecoder::ObjectArrived(const Set& op)
{
    std::cout << "An sight of an set operation arrived."
              << std::endl << std::flush;
    const Object::ListType & args = op.GetArgs();
    if (args.size() != 1) {
        std::cerr << "Funny lengthed args in create op"
                  << std::endl << std::flush;
        return;
    }
    const Object & o = args.front();

    if (!o.IsMap()) { return; }
    const Object::MapType & ent = o.AsMap();

    World & w = m_connection.getWorld();
    w.updateObject(ent);

}


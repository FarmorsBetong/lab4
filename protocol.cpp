//Protocols


enum ObjectDesc
{
Human,
NonHuman,
Vehicle,
StaticObject
};

enum ObjectForm
{
Cube,
Sphere,
Pyramid,
Cone
};

struct Coordinate
{
int x;
int y;
};

enum MsgType
{
Join, // Client joining game at server
Leave, // Client leaving game
Change, // Information to clients
Event, // Information from clients to server
TextMessage // Send text messages to one or all
};

struct MsgHead
{
unsigned int length; // Total length for whole message
unsigned int seq_no; // Sequence number
unsigned int id; // Client ID or 0;
MsgType type; // Type of message
};

 struct JoinMsg
    {
    MsgHead head;
    ObjectDesc desc;
    ObjectForm form;
    char name[MAXNAMELEN]; // nullterminated!, or empty
    };

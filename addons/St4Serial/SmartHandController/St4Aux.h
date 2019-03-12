// -----------------------------------------------------------------------------------
// ST4 aux port

#pragma once

class Ast4
{
  public:
    Ast4();
    bool guideNorth();
    bool guideSouth();
    bool guideEast();
    bool guideWest();
  private:
    int v,v1;
};

extern Ast4 auxST4;

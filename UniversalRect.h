#pragma once

class UniversalRect :
  public Gdiplus::RectF
{
public:
  UniversalRect(void);
  UniversalRect(RECT rcIn)
  {
    this->X = (Gdiplus::REAL)rcIn.left;
    this->Y = (Gdiplus::REAL)rcIn.top;
    this->Width = (Gdiplus::REAL)(rcIn.right - rcIn.left);
    this->Height = (Gdiplus::REAL)(rcIn.bottom - rcIn.top);
  }
  void GetRECT( RECT* rcIn )
  {
    rcIn->right = (LONG)(X + 0.5f) ;
    rcIn->top = (LONG)(Y + 0.5f) ;
    rcIn->left = (LONG)(X+Width + 0.5f) ;
    rcIn->bottom = (LONG)(Y+Height + 0.5f) ;
  }
  operator RECT()
  {
    RECT rc = {(LONG)(X + 0.5f), (LONG)(Y + 0.5f), (LONG)(X+Width + 0.5f), (LONG)(Y+Height + 0.5f)};
    return rc;
  }
  operator Gdiplus::Rect ()
  {
    Gdiplus::Rect rc((INT)(X + 0.5f), (INT)(Y + 0.5f), (INT)(Width + 0.5f), (INT)(Height + 0.5f));
    return rc;
  }
  ~UniversalRect(void);
};

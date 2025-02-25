#pragma once

template <class TInterface> void ReleaseInterface(TInterface* t)
{
    if (t)
    {
        t->Release();
    }
}

template <typename TComType> std::shared_ptr<TComType> SharedComPtr(TComType* p)
{
    return std::shared_ptr<TComType> (p, ReleaseInterface<TComType> );
}

template<class TMapPair> void ReleaseInterfaceMapPair( TMapPair map_pair )
{
  if ( NULL != map_pair.second )
  {
    ReleaseInterface( map_pair.second ) ;
  }
}

template <class TInterfaceMap> void ClearInterfaceMap( TInterfaceMap* the_map )
{
  std::for_each( the_map->begin(), the_map->end(), ReleaseInterfaceMapPair<TInterfaceMap::value_type> ) ;
  the_map->clear() ;
}
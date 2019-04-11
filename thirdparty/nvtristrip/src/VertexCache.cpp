

#include "VertexCache.h"


void VertexCache::Clear()
{
  for(int i = 0; i < numEntries; i++)
    entries[i] = -1;
}

void VertexCache::Copy(VertexCache* inVcache)
{
  for(int i = 0; i < numEntries; i++)
  {
    inVcache->Set(i, entries[i]);
  }
}

bool VertexCache::InCache(int entry)
{
  bool returnVal = false;

  for(int i = 0; i < numEntries; i++)
  {
    if(entries[i] == entry)
    {
      returnVal = true;
      break;
    }
  }

  return returnVal;
}


int VertexCache::AddEntry(int entry)
{
  int removed;

  removed = entries[numEntries - 1];

  //push everything right one
  for(int i = numEntries - 2; i >= 0; i--)
  {
    entries[i + 1] = entries[i];
  }

  entries[0] = entry;

  return removed;
}

  
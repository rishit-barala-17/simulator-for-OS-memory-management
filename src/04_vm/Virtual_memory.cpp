#include "virtual_memory.h"
#include <iostream>
using namespace std;



VirtualMemory::VirtualMemory(int pageCount, int frameCount, int pageSize)
    : pg_size(pageSize),
      total_pages(pageCount),
      total_frames(frameCount),
      fault_count(0),
      access_count(0)
{
    table.assign(total_pages, {false, -1});
    frame_status.assign(total_frames, 0);
}



void VirtualMemory::touch(int vaddr) 
{
    access_count++;

    int page_id = vaddr / pg_size;
    int offset = vaddr % pg_size;

    if (page_id < 0 || page_id >= total_pages) 
    {
        cout << "Virtual address error: page index invalid\n";
        return;
    }

    if (table[page_id].present) 
    {
        int phys = table[page_id].frame_index * pg_size + offset;
        cout << "Access successful | Physical address = " << phys << endl;
        return;
    }

    fault_count++;
    cout << "Page fault occurred | ";


    int frame = -1;
    for (int i = 0; i < total_frames; i++) 
    {
        if (!frame_status[i]) {
            frame = i;
            break;
        }
    }

    if (frame == -1) 
    {
        int evict_page = replace_queue.front();
        replace_queue.pop_front();
        frame = table[evict_page].frame_index;
        table[evict_page].present = false;
        cout << "Evicting page " << evict_page << " | ";
    }

    table[page_id].present = true;
    table[page_id].frame_index = frame;
    frame_status[frame] = 1;
    replace_queue.push_back(page_id);

    int phys = frame * pg_size + offset;
    cout << "Page " << page_id ;
    cout << " mapped to physical address " << phys << endl;
}



int VirtualMemory::resolve(int vaddr, bool& fault) 
{
    access_count++;

    int page_id = vaddr / pg_size;
    int offset = vaddr % pg_size;

    if (page_id < 0 || page_id >= total_pages) 
    {
        fault = false;
        return -1;
    }

    if (table[page_id].present) 
    {
        fault = false;
        return table[page_id].frame_index * pg_size + offset;
    }

    fault = true;
    fault_count++;

    int frame = -1;
    for (int i = 0; i < total_frames; i++) 
    {
        if (!frame_status[i]) 
        {
            frame = i;
            break;
        }
    }

    if (frame == -1) 
    {
        int evict_page = replace_queue.front();
        replace_queue.pop_front();
        frame = table[evict_page].frame_index;
        table[evict_page].present = false;
    }

    table[page_id].present = true;
    table[page_id].frame_index = frame;
    frame_status[frame] = 1;
    replace_queue.push_back(page_id);

    return frame * pg_size + offset;
}



void VirtualMemory::printStats() const 
{
    cout << "\n=== VM Report ===\n";
    cout << "Memory accesses: " << access_count << endl;
    cout << "Faulted pages: " << fault_count << endl;
    cout << "Hit pages: " << (access_count - fault_count) << endl;
    cout << "Fault ratio: " ;
    cout << (access_count ? 100.0 * fault_count / access_count : 0.0) ;
    cout << " %\n";
    cout << "Hit ratio: " ;
    cout << (access_count ? 100.0 * (access_count - fault_count) / access_count : 0.0) ;
    cout << " %\n";
    
    int frames_used = 0;
    for (int i = 0; i < total_frames; i++) 
    {
        if (frame_status[i]) frames_used++;
    }
    cout << "Frames utilized: " << frames_used << " / " << total_frames << endl;
    cout << "Memory utilization: " ;
    cout << (total_frames ? 100.0 * frames_used / total_frames : 0.0) ;
    cout << " %" << endl;
}


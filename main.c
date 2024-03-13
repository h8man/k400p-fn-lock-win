/**

* Goal

This program sets the function keys for the Logitech K400+ bluetooth keyboard on Windows. 
Having the F1-F12 without pressing the Fn key is nice for developers.

I wanted a simple way to do the same as the Options/Options+ software WITHOUT their 
continuously running processes.

- build with gcc:   
    `gcc main.c -o dist/k400p-fn-lock.exe -I hidapi/include -L hidapi/x86 -lhidapi`  
    `gcc main.c -D FNUNLOCK -o dist/k400p-fn-unlock.exe -I hidapi/include -L hidapi/x86 -lhidapi`

* Inspiration

code from : https://github.com/dheygere/k380-fn-lock-for-windows
values from : https://github.com/sginne/fn_key_k400_for_logitech

*/

#include <stdio.h> // printf
#include <wchar.h> // wchar_t

#include <hidapi.h>

#define MAX_STR 255

int main(int argc, char *argv[])
{
    int seq_len = 7;
    const unsigned char k400p_seq_fkeys_on[] = {0x10, 0x01, 0x09, 0x19, 0x00, 0x00, 0x00};
    const unsigned char k400p_seq_fkeys_off[] = {0x10, 0x01, 0x09, 0x18, 0x01, 0x00, 0x00};
    int k400p_vid = 0x46d;
    int k400p_pid = 0xc52b;
    static const int TARGET_USAGE = 1;
    static const int TARGET_USAGE_PAGE = 65280;

    int res;
    int result = -1;
    unsigned char buf[65];
    wchar_t wstr[MAX_STR];
    hid_device *handle;
    struct hid_device_info *devs, *cur_dev;

    printf("Initialize the hidapi library\n");
    res = hid_init();

    devs = hid_enumerate(k400p_vid, k400p_pid);
    cur_dev = devs;
	printf("Enuming hidapi dvices..\n");
    while (cur_dev)
    {
		printf("%s\n",cur_dev->path);
        if (cur_dev->usage == TARGET_USAGE && cur_dev->usage_page == TARGET_USAGE_PAGE)
        {
            handle = hid_open_path(cur_dev->path);

#ifdef FNUNLOCK
            printf("Set media keys as default\n");
            res = hid_write(handle, k400p_seq_fkeys_off, seq_len);
#else
            printf("Set function keys as default\n");
            res = hid_write(handle, k400p_seq_fkeys_on, seq_len);
#endif

            if (res != seq_len)
            {
                printf("error: %ls\n", hid_error(handle));
            }

            hid_close(handle);
            if (res < 0)
            {
                result = -1;
                break;
            }
            else
            {
                result = 0;
                break;
            }
        }
        cur_dev = cur_dev->next;
    }
    hid_free_enumeration(devs);

    hid_exit();
    return result;
}
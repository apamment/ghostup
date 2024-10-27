#pragma once

#pragma pack(push, 1)

struct net_header_rec {
    uint16_t  tosys,    /* destination system */
        touser,     /* destination user */
        fromsys,    /* originating system */
        fromuser,   /* originating user */
        main_type,  /* main message type */
        minor_type, /* minor message type */
        list_len;   /* # of entries in system list */
    uint32_t   daten,    /* date/time sent */
        length;   /* # of bytes of msg after header */
    uint16_t  method;    /* method of compression */
};

#pragma pack(pop)
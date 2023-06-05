// #include 

enum {
    LCDMNG_WELCOME,
    LCDMNG_GUIDE, 
    LCDMNG_PRICE, 
    LCDMNG_WRN_LOW_CARD, 
    LCDMNG_WRN_REMAIN,
    LCDMNG_ERR_EMPTY_CARD, 
    LCDMNG_RECEIVED,
    LCDMNG_OK, 
    LCDMNG_ERR_CASH, 
    LCDMNG_PAYING_CARD,
    LCDMNG_ERR_CARD, 
    LCDMNG_THANKYOU, 
    LCDMNG_SORRY, 
    LCDMNG_EXIT
}

static const char * lcd_display_table[] = {
    [LCDMNG_WELCOME] = "Hello %s",
    [LCDMNG_GUIDE] = "Put money in slot",
    [LCDMNG_PRICE] = "%d per card",
    [LCDMNG_WRN_LOW_CARD] = "low card",
    [LCDMNG_WRN_REMAIN] = "remain %d cards ", 
    [LCDMNG_ERR_EMPTY_CARD] = "Out of card",
    [LCDMNG_RECEIVED] = "Received %d cards",
    [LCDMNG_OK] = "OK",
    [LCDMNG_ERR_CASH] = "Retry other cash", 
    [LCDMNG_PAYING_CARD] = "Paying %d / %d", 
    [LCDMNG_ERR_CARD] = "Card error %d",
    [LCDMNG_THANKYOU] = "Thankyou",
    [LCDMNG_SORRY] = "Sorry, system is maintaining", 
    [LCDMNG_EXIT] =  "Bye and see you again!"
}
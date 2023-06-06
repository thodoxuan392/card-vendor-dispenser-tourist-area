
#include "DeviceManager/lcdmanager.h"
#include "Device/lcd.h"

enum
{
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
};

static const char *lcd_display_table[] = {
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
    [LCDMNG_EXIT] = "Bye and see you again!"};

enum
{
    LCDMNG_TECH_WELCOME = 0,
    LCDMNG_TECH_PASSWORD,
    LCDMNG_TECH_PASS_FAIL,
    LCDMNG_TECH_PRICE,
    LCDMNG_TECH_CONFIRM,
    LCDMNG_TECH_CANCELLED,
    LCDMNG_TECH_SET_OK,
    LCDMNG_TECH_SET_FAIL,
};

static const char *
    lcd_display_table_technician[] = {
        [LCDMNG_TECH_WELCOME] = "welcome techinician \r\n please enter password in %d s",
        [LCDMNG_TECH_PASSWORD] = "********",
        [LCDMNG_TECH_PASS_FAIL] = "Password fail -> exit"[LCDMNG_TECH_PRICE] = "Price %dk per card",
        [LCDMNG_TECH_CONFIRM] = "Setting not set",
        [LCDMNG_TECH_CANCELLED] = "Setting saved",
        [LCDMNG_TECH_SET_OK] = "Setting OK",
        [LCDMNG_TECH_SET_FAIL] = "Setting Fail",
};

// enum
// {
//     LCDMNG_IDLE,
//     LCDMNG_RESETING,
//     LCDMNG_WAIT_FOR_RESETING,
//     LCDMNG_PAYOUTING,
//     LCDMNG_WAIT_FOR_PAYOUTING,
//     LCDMNG_CALLBACKING,
//     LCDMNG_WAIT_FOR_CALLBACKING,
//     LCDMNG_ERROR
// };

static uint8_t lcdmng_state = LCDMNG_IDLE;
static bool lcdmng_is_technician = false;

// Private function
void LCDMNG_init()
{
    LCD_init();
    // void LCD_clear_screen();
    // void LCD_display_str(char * fmt, ...);
    // bool LCD_test();
}

void LCDMNG_run()
{
    if (lcdmng_is_technician)
    {
        switch (lcdmng_state)
        {
            {
            case LCDMNG_WELCOME:
                LCD_display_str(lcd_display_table[lcdmng_state]);
                break;
            case LCDMNG_GUIDE /* constant-expression */:
                LCD_display_str(lcd_display_table[lcdmng_state]);
                break;
            case LCDMNG_PRICE /* constant-expression */:
                LCD_display_str(lcd_display_table[lcdmng_state]);
                break;
            case LCDMNG_WRN_LOW_CARD /* constant-expression */:
                LCD_display_str(lcd_display_table[lcdmng_state]);
                break;
            case LCDMNG_WRN_REMAIN /* constant-expression */:
                LCD_display_str(lcd_display_table[lcdmng_state]);
                break;
            case LCDMNG_ERR_EMPTY_CARD /* constant-expression */:
                LCD_display_str(lcd_display_table[lcdmng_state]);
                break;
            case LCDMNG_RECEIVED /* constant-expression */:
                LCD_display_str(lcd_display_table[lcdmng_state]);
                break;
            case LCDMNG_OK /* constant-expression */:
                LCD_display_str(lcd_display_table[lcdmng_state]);
                break;
            case LCDMNG_ERR_CASH /* constant-expression */:
                LCD_display_str(lcd_display_table[lcdmng_state]);
                break;
            case LCDMNG_PAYING_CARD /* constant-expression */:
                LCD_display_str(lcd_display_table[lcdmng_state]);
                break;
            case LCDMNG_ERR_CARD /* constant-expression */:
                LCD_display_str(lcd_display_table[lcdmng_state]);
                break;
            case LCDMNG_THANKYOU /* constant-expression */:
                LCD_display_str(lcd_display_table[lcdmng_state]);
                break;
            case LCDMNG_SORRY /* constant-expression */:
                LCD_display_str(lcd_display_table[lcdmng_state]);
                break;
            case LCDMNG_EXIT /* constant-expression */:
                LCD_display_str(lcd_display_table[lcdmng_state]);
                break;
            default:
                lcdmng_state = LCDMNG_WELCOME;
                break;
            }
        }
    }

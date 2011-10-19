typedef struct {
    unsigned long int dc_est;
    signed char prev;
    unsigned char run_len;
    enum {invalid, ambiguous, bit, clock} state;
    void (*decoded_bit_callback)(signed char);
} manc_diff_decoder;

void manc_diff_decoder_init(manc_diff_decoder *m, void (*decoded_bit_callback)(signed char));


void manc_decode(manc_diff_decoder *m, unsigned char adc8);

  
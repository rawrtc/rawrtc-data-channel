#pragma once

#include <stdbool.h> // bool

// TODO: Make this a build configuration
#define RAWRTC_DEBUG_LEVEL 5

#define HAVE_INTTYPES_H
#include <re.h>
#include <usrsctp.h>
#include <rawrtcc.h>

/*
 * Version
 *
 * Follows Semantic Versioning 2.0.0,
 * see: https://semver.org
 *
 * TODO: Find a way to keep this in sync with the one in CMakeLists.txt
 */
#define RAWRTCDC_VERSION "0.0.1"

/*
 * Data channel is unordered bit flag.
 */
enum {
    RAWRTC_DATA_CHANNEL_TYPE_IS_UNORDERED = 0x80
};

/*
 * Data channel types.
 */
enum rawrtc_data_channel_type {
    RAWRTC_DATA_CHANNEL_TYPE_RELIABLE_ORDERED = 0x00,
    RAWRTC_DATA_CHANNEL_TYPE_RELIABLE_UNORDERED = 0x80,
    RAWRTC_DATA_CHANNEL_TYPE_UNRELIABLE_ORDERED_RETRANSMIT = 0x01,
    RAWRTC_DATA_CHANNEL_TYPE_UNRELIABLE_UNORDERED_RETRANSMIT = 0x81,
    RAWRTC_DATA_CHANNEL_TYPE_UNRELIABLE_ORDERED_TIMED = 0x02,
    RAWRTC_DATA_CHANNEL_TYPE_UNRELIABLE_UNORDERED_TIMED = 0x82
}; // IMPORTANT: If you add a new type, ensure that every data channel transport handles it
   //            correctly! Also, ensure this still works with the unordered bit flag above or
   //            update the implementations.

/*
 * Data channel message flags.
 */
enum rawrtc_data_channel_message_flag {
    RAWRTC_DATA_CHANNEL_MESSAGE_FLAG_NONE = 1 << 0,
    RAWRTC_DATA_CHANNEL_MESSAGE_FLAG_IS_ABORTED = 1 << 1,
    RAWRTC_DATA_CHANNEL_MESSAGE_FLAG_IS_COMPLETE = 1 << 2,
    RAWRTC_DATA_CHANNEL_MESSAGE_FLAG_IS_BINARY = 1 << 3,
};

/*
 * SCTP transport state.
 */
enum rawrtc_sctp_transport_state {
    RAWRTC_SCTP_TRANSPORT_STATE_NEW,
    RAWRTC_SCTP_TRANSPORT_STATE_CONNECTING,
    RAWRTC_SCTP_TRANSPORT_STATE_CONNECTED,
    RAWRTC_SCTP_TRANSPORT_STATE_CLOSED
};

/*
 * Data channel state.
 */
enum rawrtc_data_channel_state {
    RAWRTC_DATA_CHANNEL_STATE_CONNECTING,
    RAWRTC_DATA_CHANNEL_STATE_OPEN,
    RAWRTC_DATA_CHANNEL_STATE_CLOSING,
    RAWRTC_DATA_CHANNEL_STATE_CLOSED
};

#ifdef SCTP_REDIRECT_TRANSPORT
/*
 * SCTP redirect transport states.
 */
enum rawrtc_sctp_redirect_transport_state {
    RAWRTC_SCTP_REDIRECT_TRANSPORT_STATE_NEW,
    RAWRTC_SCTP_REDIRECT_TRANSPORT_STATE_OPEN,
    RAWRTC_SCTP_REDIRECT_TRANSPORT_STATE_CLOSED
};
#endif



/*
 * SCTP transport state change handler.
 */
typedef void (rawrtc_sctp_transport_state_change_handler)(
    enum rawrtc_sctp_transport_state const state,
    void* const arg
);

/*
 * Data channel open handler.
 */
typedef void (rawrtc_data_channel_open_handler)(
    void* const arg
);

/*
 * Data channel buffered amount low handler.
 */
typedef void (rawrtc_data_channel_buffered_amount_low_handler)(
    void* const arg
);

/*
 * Data channel error handler.
 */
typedef void (rawrtc_data_channel_error_handler)(
    /* TODO */
    void* const arg
);

/*
 * Data channel close handler.
 */
typedef void (rawrtc_data_channel_close_handler)(
    void* const arg
);

/*
 * Data channel message handler.
 *
 * Note: `buffer` may be NULL in case partial delivery has been
 *       requested and a message has been aborted (this can only happen
 *       on partially reliable channels).
 */
typedef void (rawrtc_data_channel_message_handler)(
    struct mbuf* const buffer, // nullable (in case partial delivery has been requested)
    enum rawrtc_data_channel_message_flag const flags,
    void* const arg
);

/*
 * Data channel handler.
 *
 * You should call `rawrtc_data_channel_set_options` in this handler
 * before doing anything else if you want to change behaviour of the
 * data channel.
 */
typedef void (rawrtc_data_channel_handler)(
    struct rawrtc_data_channel* const data_channel, // read-only, MUST be referenced when used
    void* const arg
);



#ifdef SCTP_REDIRECT_TRANSPORT
/*
 * Redirect transport.
 */
struct rawrtc_sctp_redirect_transport;
#endif

/*
 * SCTP transport.
 */
struct rawrtc_sctp_transport;

/*
 * SCTP data channel context.
 */
struct rawrtc_sctp_data_channel_context;

/*
 * Data channel.
 */
struct rawrtc_data_channel;

/*
 * Generic data transport.
 */
struct rawrtc_data_transport;

/*
 * Data channel parameters.
 */
struct rawrtc_data_channel_parameters;

/*
 * Data channel options.
 */
struct rawrtc_data_channel_options;

/*
 * SCTP capabilities.
 */
struct rawrtc_sctp_capabilities;



/*
 * Initialise rawrtcdc. Must be called before making a call to any
 * other function.
 *
 * Note: In case `init_re` is not set to `true`, you MUST initialise
 *       re yourselves before calling this function.
 */
enum rawrtc_code rawrtcdc_init(
    bool const init_re
);

/*
 * Close rawrtcdc and free up all resources.
 *
 * Note: In case `close_re` is not set to `true`, you MUST close
 *       re yourselves.
 */
enum rawrtc_code rawrtcdc_close(
    bool const close_re
);

#ifdef SCTP_REDIRECT_TRANSPORT
/*
 * Create an SCTP redirect transport.
 * `*transportp` must be unreferenced.
 */
enum rawrtc_code rawrtc_sctp_redirect_transport_create(
    struct rawrtc_sctp_redirect_transport** const transportp, // de-referenced
    struct rawrtc_dtls_transport* const dtls_transport, // referenced
    uint16_t const port, // zeroable
    char* const redirect_ip, // copied
    uint16_t const redirect_port
);

/*
 * Start an SCTP redirect transport.
 */
enum rawrtc_code rawrtc_sctp_redirect_transport_start(
    struct rawrtc_sctp_redirect_transport* const transport,
    struct rawrtc_sctp_capabilities const * const remote_capabilities, // copied
    uint16_t remote_port // zeroable
);

/*
 * Stop and close the SCTP redirect transport.
 */
enum rawrtc_code rawrtc_sctp_redirect_transport_stop(
    struct rawrtc_sctp_redirect_transport* const transport
);

/*
 * Get the redirected local SCTP port of the SCTP redirect transport.
 */
enum rawrtc_code rawrtc_sctp_redirect_transport_get_port(
    uint16_t* const portp, // de-referenced
    struct rawrtc_sctp_redirect_transport* const transport
);

/*
 * TODO:
 * rawrtc_sctp_redirect_transport_get_state
 */
#endif

/*
 * Create a new SCTP transport capabilities instance.
 * `*capabilitiesp` must be unreferenced.
 */
enum rawrtc_code rawrtc_sctp_capabilities_create(
    struct rawrtc_sctp_capabilities** const capabilitiesp, // de-referenced
    uint64_t const max_message_size
);

/*
 * Get the SCTP parameter's maximum message size value.
 */
enum rawrtc_code rawrtc_sctp_capabilities_get_max_message_size(
    uint64_t* const max_message_sizep, // de-referenced
    struct rawrtc_sctp_capabilities* const capabilities
);

/*
 * Get the corresponding name for an SCTP transport state.
 */
char const * const rawrtc_sctp_transport_state_to_name(
    enum rawrtc_sctp_transport_state const state
);

/*
 * Create an SCTP transport.
 * `*transportp` must be unreferenced.
 */
enum rawrtc_code rawrtc_sctp_transport_create(
    struct rawrtc_sctp_transport** const transportp, // de-referenced
    struct rawrtc_dtls_transport* const dtls_transport, // referenced
    uint16_t port, // zeroable
    rawrtc_data_channel_handler* const data_channel_handler, // nullable
    rawrtc_sctp_transport_state_change_handler* const state_change_handler, // nullable
    void* const arg // nullable
);

/*
 * Get the SCTP data transport instance.
 * `*transportp` must be unreferenced.
 */
enum rawrtc_code rawrtc_sctp_transport_get_data_transport(
    struct rawrtc_data_transport** const transportp, // de-referenced
    struct rawrtc_sctp_transport* const sctp_transport // referenced
);

/*
 * Start the SCTP transport.
 */
enum rawrtc_code rawrtc_sctp_transport_start(
    struct rawrtc_sctp_transport* const transport,
    struct rawrtc_sctp_capabilities const * const remote_capabilities, // copied
    uint16_t remote_port // zeroable
);

/*
 * Stop and close the SCTP transport.
 */
enum rawrtc_code rawrtc_sctp_transport_stop(
    struct rawrtc_sctp_transport* const transport
);

/*
 * TODO (from RTCSctpTransport interface)
 * rawrtc_sctp_transport_get_transport
 * rawrtc_sctp_transport_get_state
 */

/*
 * Get the local port of the SCTP transport.
 */
enum rawrtc_code rawrtc_sctp_transport_get_port(
    uint16_t* const portp, // de-referenced
    struct rawrtc_sctp_transport* const transport
);

/*
 * Get the local SCTP transport capabilities (static).
 * `*capabilitiesp` must be unreferenced.
 */
enum rawrtc_code rawrtc_sctp_transport_get_capabilities(
    struct rawrtc_sctp_capabilities** const capabilitiesp // de-referenced
);

/*
 * TODO (from RTCSctpTransport interface)
 * rawrtc_sctp_transport_set_data_channel_handler
 */

/*
 * Create data channel parameters.
 *
 * For `RAWRTC_DATA_CHANNEL_TYPE_RELIABLE_*`, the reliability parameter
 * is being ignored.
 *
 * When using `RAWRTC_DATA_CHANNEL_TYPE_*_RETRANSMIT`, the reliability
 * parameter specifies the number of times a retransmission occurs if
 * not acknowledged before the message is being discarded.
 *
 * When using `RAWRTC_DATA_CHANNEL_TYPE_*_TIMED`, the reliability
 * parameter specifies the time window in milliseconds during which
 * (re-)transmissions may occur before the message is being discarded.
 *
 * `*parametersp` must be unreferenced.
 *
 * In case `negotiated` is set to `false`, the `id` is being ignored.
 */
enum rawrtc_code rawrtc_data_channel_parameters_create(
    struct rawrtc_data_channel_parameters** const parametersp, // de-referenced
    char const * const label, // copied, nullable
    enum rawrtc_data_channel_type const channel_type,
    uint32_t const reliability_parameter,
    char const * const protocol, // copied
    bool const negotiated,
    uint16_t const id
);

/*
 * Get the label from the data channel parameters.
 * `*labelp` will be set to a copy of the parameter's label and must be
 * unreferenced.
 *
 * Return `RAWRTC_CODE_NO_VALUE` in case no label has been set.
 * Otherwise, `RAWRTC_CODE_SUCCESS` will be returned and `*parameters*
 * must be unreferenced.
 */
enum rawrtc_code rawrtc_data_channel_parameters_get_label(
    char** const labelp, // de-referenced
    struct rawrtc_data_channel_parameters* const parameters
);

/*
 * TODO
 * rawrtc_data_channel_parameters_get_channel_type
 * rawrtc_data_channel_parameters_get_reliability_parameter
 */

/*
 * Get the protocol from the data channel parameters.
 * `*protocolp` will be set to a copy of the parameter's protocol and
 * must be unreferenced.
 *
 * Return `RAWRTC_CODE_NO_VALUE` in case no protocol has been set.
 * Otherwise, `RAWRTC_CODE_SUCCESS` will be returned and `*protocolp*
 * must be unreferenced.
 */
enum rawrtc_code rawrtc_data_channel_parameters_get_protocol(
    char** const protocolp, // de-referenced
    struct rawrtc_data_channel_parameters* const parameters
);

/*
 * TODO
 * rawrtc_data_channel_parameters_get_negotiated
 * rawrtc_data_channel_parameters_get_id
 */

/*
 * Create data channel options.
 *
 * `*optionsp` must be unreferenced.
 *
 * If `deliver_partially` is set to `true`, you will receive partial
 * messages. If set to `false`, messages will be reassembled before
 * delivery. If enabled, message chunks will be delivered until the
 * message is complete. Other messages' chunks WILL NOT be interleaved
 * on the same channel.
 */
enum rawrtc_code rawrtc_data_channel_options_create(
    struct rawrtc_data_channel_options** const optionsp, // de-referenced
    bool const deliver_partially
);

/*
 * Get the corresponding name for a data channel state.
 */
char const * const rawrtc_data_channel_state_to_name(
    enum rawrtc_data_channel_state const state
);

/*
 * Create a data channel.
 * `*channelp` must be unreferenced.
 */
enum rawrtc_code rawrtc_data_channel_create(
    struct rawrtc_data_channel** const channelp, // de-referenced
    struct rawrtc_data_transport* const transport, // referenced
    struct rawrtc_data_channel_parameters* const parameters, // referenced
    struct rawrtc_data_channel_options* const options, // nullable, referenced
    rawrtc_data_channel_open_handler* const open_handler, // nullable
    rawrtc_data_channel_buffered_amount_low_handler* const buffered_amount_low_handler, // nullable
    rawrtc_data_channel_error_handler* const error_handler, // nullable
    rawrtc_data_channel_close_handler* const close_handler, // nullable
    rawrtc_data_channel_message_handler* const message_handler, // nullable
    void* const arg // nullable
);

/*
 * Set the argument of a data channel that is passed to the various
 * handlers.
 */
enum rawrtc_code rawrtc_data_channel_set_arg(
    struct rawrtc_data_channel* const channel,
    void* const arg // nullable
);

/*
 * Set options on a data channel.
 *
 * Note: This function must be called directly after creation of the
 * data channel (either by explicitly creating it or implicitly in form
 * of the data channel handler callback) and before calling any other
 * data channel function.
 */
enum rawrtc_code rawrtc_data_channel_set_options(
    struct rawrtc_data_channel* const channel,
    struct rawrtc_data_channel_options* options // nullable, referenced
);

/*
 * Send data via the data channel.
 */
enum rawrtc_code rawrtc_data_channel_send(
    struct rawrtc_data_channel* const channel,
    struct mbuf* const buffer, // nullable (if empty message), referenced
    bool const is_binary
);

/*
 * Close the data channel.
 */
enum rawrtc_code rawrtc_data_channel_close(
    struct rawrtc_data_channel* const channel
);

/*
 * TODO (from RTCDataChannel interface)
 * rawrtc_data_channel_get_transport
 * rawrtc_data_channel_get_ready_state
 * rawrtc_data_channel_get_buffered_amount
 * rawrtc_data_channel_get_buffered_amount_low_threshold
 * rawrtc_data_channel_set_buffered_amount_low_threshold
 */

/*
 * Unset the handler argument and all handlers of the data channel.
 */
enum rawrtc_code rawrtc_data_channel_unset_handlers(
    struct rawrtc_data_channel* const channel
);

/*
 * Get the data channel's parameters.
 * `*parametersp` must be unreferenced.
 */
enum rawrtc_code rawrtc_data_channel_get_parameters(
    struct rawrtc_data_channel_parameters** const parametersp, // de-referenced
    struct rawrtc_data_channel* const channel
);

/*
 * Set the data channel's open handler.
 */
enum rawrtc_code rawrtc_data_channel_set_open_handler(
    struct rawrtc_data_channel* const channel,
    rawrtc_data_channel_open_handler* const open_handler // nullable
);

/*
 * Get the data channel's open handler.
 * Returns `RAWRTC_CODE_NO_VALUE` in case no handler has been set.
 */
enum rawrtc_code rawrtc_data_channel_get_open_handler(
    rawrtc_data_channel_open_handler** const open_handlerp, // de-referenced
    struct rawrtc_data_channel* const channel
);

/*
 * Set the data channel's buffered amount low handler.
 */
enum rawrtc_code rawrtc_data_channel_set_buffered_amount_low_handler(
    struct rawrtc_data_channel* const channel,
    rawrtc_data_channel_buffered_amount_low_handler* const buffered_amount_low_handler // nullable
);

/*
 * Get the data channel's buffered amount low handler.
 * Returns `RAWRTC_CODE_NO_VALUE` in case no handler has been set.
 */
enum rawrtc_code rawrtc_data_channel_get_buffered_amount_low_handler(
    rawrtc_data_channel_buffered_amount_low_handler** const buffered_amount_low_handlerp, // de-referenced
    struct rawrtc_data_channel* const channel
);

/*
 * Set the data channel's error handler.
 */
enum rawrtc_code rawrtc_data_channel_set_error_handler(
    struct rawrtc_data_channel* const channel,
    rawrtc_data_channel_error_handler* const error_handler // nullable
);

/*
 * Get the data channel's error handler.
 * Returns `RAWRTC_CODE_NO_VALUE` in case no handler has been set.
 */
enum rawrtc_code rawrtc_data_channel_get_error_handler(
    rawrtc_data_channel_error_handler** const error_handlerp, // de-referenced
    struct rawrtc_data_channel* const channel
);

/*
 * Set the data channel's close handler.
 */
enum rawrtc_code rawrtc_data_channel_set_close_handler(
    struct rawrtc_data_channel* const channel,
    rawrtc_data_channel_close_handler* const close_handler // nullable
);

/*
 * Get the data channel's close handler.
 * Returns `RAWRTC_CODE_NO_VALUE` in case no handler has been set.
 */
enum rawrtc_code rawrtc_data_channel_get_close_handler(
    rawrtc_data_channel_close_handler** const close_handlerp, // de-referenced
    struct rawrtc_data_channel* const channel
);

/*
 * Set the data channel's message handler.
 */
enum rawrtc_code rawrtc_data_channel_set_message_handler(
    struct rawrtc_data_channel* const channel,
    rawrtc_data_channel_message_handler* const message_handler // nullable
);

/*
 * Get the data channel's message handler.
 * Returns `RAWRTC_CODE_NO_VALUE` in case no handler has been set.
 */
enum rawrtc_code rawrtc_data_channel_get_message_handler(
    rawrtc_data_channel_message_handler** const message_handlerp, // de-referenced
    struct rawrtc_data_channel* const channel
);

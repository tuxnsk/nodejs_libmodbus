
#include <v8.h>
#include <node.h>
#include <node_version.h>
#include <uv.h>
#include <modbus.h>
#include <string.h>
#include <errno.h>
#include <unistd.h> 
#include <sys/socket.h>

#include <iostream>

using namespace v8;
using namespace node;
using namespace std;

// finger to the sky
#define REPORT_LEN 0xFF

#if (NODE_MODULE_VERSION < 12)

#define TO_EXTERNAL(sb) \
    External::Wrap(sb)
#define FROM_EXTERNAL(sb) \
    External::Unwrap(sb)

#else

#define TO_EXTERNAL(sb) \
    External::New(v8::Isolate::GetCurrent(), sb)
#define FROM_EXTERNAL(sb) \
    External::Cast(*sb)->Value()

#endif

// modbus_t *modbus_new_rtu(const char *device, int baud, char parity, int data_bit, int stop_bit);
// External new_rtu(String, Integer, String, Integer, Integer);
void js_new_rtu(const v8::FunctionCallbackInfo<v8::Value>& args) {
	String::Utf8Value device(args[0]);
	int baud = Local<Integer>::Cast(args[1])->Int32Value();
	String::Utf8Value parity_str(args[2]);
	int data_bit = Local<Integer>::Cast(args[3])->Int32Value();
	int stop_bit = Local<Integer>::Cast(args[4])->Int32Value();
	
	char parity = (*parity_str)[0];
	
	modbus_t *ctx = modbus_new_rtu(*device, baud, parity, data_bit, stop_bit);
	
	if (ctx == NULL) {
		args.GetReturnValue().SetNull();
	} else {
		args.GetReturnValue().Set(TO_EXTERNAL(ctx));
	}
}

// int modbus_rtu_get_serial_mode(modbus_t *ctx);
// Integer rtu_get_serial_mode(External);
void js_rtu_get_serial_mode(const v8::FunctionCallbackInfo<v8::Value>& args) {
	modbus_t *ctx = static_cast<modbus_t *>(FROM_EXTERNAL(args[0]));
	
	int ret = modbus_rtu_get_serial_mode(ctx);
	
	args.GetReturnValue().Set(ret);
}

// int modbus_rtu_set_serial_mode(modbus_t *ctx, int mode);
// Integer rtu_set_serial_mode(External, Integer);
void js_rtu_set_serial_mode(const v8::FunctionCallbackInfo<v8::Value>& args) {
	modbus_t *ctx = static_cast<modbus_t *>(FROM_EXTERNAL(args[0]));
	int mode = Local<Integer>::Cast(args[1])->Int32Value();
	
	int ret = modbus_rtu_set_serial_mode(ctx, mode);
	
	args.GetReturnValue().Set(ret);
}

/*
// int modbus_rtu_get_rts(modbus_t *ctx);
void js_rtu_get_rts(const v8::FunctionCallbackInfo<v8::Value>& args) {
	modbus_t *ctx = static_cast<modbus_t *>(FROM_EXTERNAL(args[0]));
	
	int ret = modbus_rtu_get_rts(ctx);
	
	args.GetReturnValue().Set(ret);
}

// int modbus_rtu_set_rts(modbus_t *ctx, int mode)
void js_rtu_set_rts(const v8::FunctionCallbackInfo<v8::Value>& args) {
	modbus_t *ctx = static_cast<modbus_t *>(FROM_EXTERNAL(args[0]));
	int mode = Local<Integer>::Cast(args[1])->Int32Value();
	
	int ret = modbus_rtu_set_rts(ctx, mode);
	
	args.GetReturnValue().Set(ret);
}
*/

// modbus_t *modbus_new_tcp(const char *ip, int port);
// External new_tcp(String, Integer);
void js_new_tcp(const v8::FunctionCallbackInfo<v8::Value>& args) {
	String::Utf8Value ip(args[0]);
	int port = Local<Integer>::Cast(args[1])->Int32Value();
	
	modbus_t *ctx = modbus_new_tcp(*ip, port);
	
	if (ctx == NULL) {
		args.GetReturnValue().SetNull();
	} else {
		args.GetReturnValue().Set(TO_EXTERNAL(ctx));
	}
}

// modbus_t *modbus_new_tcp_pi(const char *node, const char *service);
// External new_tcp_pi(String, String);
void js_new_tcp_pi(const v8::FunctionCallbackInfo<v8::Value>& args) {
	String::Utf8Value node(args[0]);
	String::Utf8Value service(args[1]);
	
	modbus_t *ctx = modbus_new_tcp_pi(*node, *service);
	
	if (ctx == NULL) {
		args.GetReturnValue().SetNull();
	} else {
		args.GetReturnValue().Set(TO_EXTERNAL(ctx));
	}
}

// void modbus_free(modbus_t *ctx);
// Undefined free(External);
void js_free(const v8::FunctionCallbackInfo<v8::Value>& args) {
	modbus_t *ctx = static_cast<modbus_t *>(FROM_EXTERNAL(args[0]));
	
	modbus_free(ctx);
	
	args.GetReturnValue().SetUndefined();
}

// void modbus_get_byte_timeout(modbus_t *ctx, struct timeval *timeout);
// Undefined get_byte_timeout(External, Object);
void js_get_byte_timeout(const v8::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = v8::Isolate::GetCurrent();
	HandleScope scope(isolate);
	modbus_t *ctx = static_cast<modbus_t *>(FROM_EXTERNAL(args[0]));
	Local<Object> timeout_obj = Local<Object>::Cast(args[1]);
	

	uint32_t sec = 0;
	uint32_t usec = 0;

	modbus_get_byte_timeout(ctx, &sec, &usec);

	struct timeval timeout;
	timeout.tv_sec = sec;
	timeout.tv_usec = usec;
	
	timeout_obj->Set(String::NewFromUtf8(isolate, "tv_sec"), Uint32::New(isolate, timeout.tv_sec));
	timeout_obj->Set(String::NewFromUtf8(isolate, "tv_usec"), Uint32::New(isolate, timeout.tv_usec));
	
	args.GetReturnValue().SetUndefined();
}

// void modbus_set_byte_timeout(modbus_t *ctx, struct timeval *timeout);
// Undefined set_byte_timeout(External, Object);
void js_set_byte_timeout(const v8::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = v8::Isolate::GetCurrent();
	HandleScope scope(isolate);
	modbus_t *ctx = static_cast<modbus_t *>(FROM_EXTERNAL(args[0]));
	Local<Object> timeout_obj = Local<Object>::Cast(args[1]);
	
	struct timeval timeout;
	timeout.tv_sec = timeout_obj->Get(String::NewFromUtf8(isolate, "tv_sec"))->Uint32Value();
	timeout.tv_usec = timeout_obj->Get(String::NewFromUtf8(isolate, "tv_usec"))->Uint32Value();

	modbus_set_byte_timeout(ctx, timeout.tv_sec, timeout.tv_usec);
	
	args.GetReturnValue().SetUndefined();
}

// void modbus_set_debug(modbus_t *ctx, int boolean);
// Undefined set_debug(External, Integer);
void js_set_debug(const v8::FunctionCallbackInfo<v8::Value>& args) {
	modbus_t *ctx = static_cast<modbus_t *>(FROM_EXTERNAL(args[0]));
	int boolean = Local<Integer>::Cast(args[1])->Int32Value();
	
	modbus_set_debug(ctx, boolean);
	
	args.GetReturnValue().SetUndefined();
}

// int modbus_set_error_recovery(modbus_t *ctx, modbus_error_recovery_mode error_recovery);
// Integer set_error_recovery(External, Integer);
void js_set_error_recovery(const v8::FunctionCallbackInfo<v8::Value>& args) {
	modbus_t *ctx = static_cast<modbus_t *>(FROM_EXTERNAL(args[0]));
	int error_recovery = Local<Integer>::Cast(args[1])->Int32Value();
	
	int ret = modbus_set_error_recovery(ctx, static_cast<modbus_error_recovery_mode>(error_recovery));
	
	args.GetReturnValue().Set(ret);
}

// int modbus_get_header_length(modbus_t *ctx);
// Integer get_header_length(External);
void js_get_header_length(const v8::FunctionCallbackInfo<v8::Value>& args) {
	modbus_t *ctx = static_cast<modbus_t *>(FROM_EXTERNAL(args[0]));
	
	int ret = modbus_get_header_length(ctx);
	
	args.GetReturnValue().Set(ret);
}

// void modbus_get_response_timeout(modbus_t *ctx, struct timeval *timeout);
// Undefined get_response_timeout(External, Object);
void js_get_response_timeout(const v8::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = v8::Isolate::GetCurrent();
	HandleScope scope(isolate);
	modbus_t *ctx = static_cast<modbus_t *>(FROM_EXTERNAL(args[0]));
	Local<Object> timeout_obj = Local<Object>::Cast(args[1]);
	
	uint32_t sec = 0;
	uint32_t usec = 0;

	modbus_get_response_timeout(ctx, &sec, &usec);

	struct timeval timeout;
	timeout.tv_sec = sec;
	timeout.tv_usec = usec;
	
	timeout_obj->Set(String::NewFromUtf8(isolate, "tv_sec"), Uint32::New(isolate, timeout.tv_sec));
	timeout_obj->Set(String::NewFromUtf8(isolate, "tv_usec"), Uint32::New(isolate, timeout.tv_usec));
	
	args.GetReturnValue().SetUndefined();
}

// void modbus_set_response_timeout(modbus_t *ctx, struct timeval *timeout);
// Undefined set_response_timeout(External, Object);
void js_set_response_timeout(const v8::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = v8::Isolate::GetCurrent();
	HandleScope scope(isolate);
	modbus_t *ctx = static_cast<modbus_t *>(FROM_EXTERNAL(args[0]));
	Local<Object> timeout_obj = Local<Object>::Cast(args[1]);
	
	struct timeval timeout;
	timeout.tv_sec = timeout_obj->Get(String::NewFromUtf8(isolate, "tv_sec"))->Uint32Value();
	timeout.tv_usec = timeout_obj->Get(String::NewFromUtf8(isolate, "tv_usec"))->Uint32Value();
	modbus_set_response_timeout(ctx, timeout.tv_sec, timeout.tv_usec);
	
	args.GetReturnValue().SetUndefined();
}

// int modbus_set_slave(modbus_t *ctx, int slave);
// Integer set_slave(External, Integer);
void js_set_slave(const v8::FunctionCallbackInfo<v8::Value>& args) {
	modbus_t *ctx = static_cast<modbus_t *>(FROM_EXTERNAL(args[0]));
	int slave = Local<Integer>::Cast(args[1])->Int32Value();
	
	int ret = modbus_set_slave(ctx, slave);
	
	args.GetReturnValue().Set(ret);
}

// void modbus_set_socket(modbus_t *ctx, int socket);
// Undefined set_socket(External, Integer);
void js_set_socket(const v8::FunctionCallbackInfo<v8::Value>& args) {
	modbus_t *ctx = static_cast<modbus_t *>(FROM_EXTERNAL(args[0]));
	int socket = Local<Integer>::Cast(args[1])->Int32Value();
	
	modbus_set_socket(ctx, socket);
	
	args.GetReturnValue().SetUndefined();
}

// int modbus_get_socket(modbus_t *ctx);
// Integer get_socket(External);
void js_get_socket(const v8::FunctionCallbackInfo<v8::Value>& args) {
	modbus_t *ctx = static_cast<modbus_t *>(FROM_EXTERNAL(args[0]));
	
	int ret = modbus_get_socket(ctx);
	
	args.GetReturnValue().Set(ret);
}

// in js module
// void modbus_set_bits_from_byte(uint8_t *dest, int index, const uint8_t value);
// void modbus_set_bits_from_bytes(uint8_t *dest, int index, unsigned int nb_bits, const uint8_t *tab_byte);
// uint8_t modbus_get_byte_from_bits(const uint8_t *src, int index, unsigned int nb_bits);
// float modbus_get_float(const uint16_t *src);
// void modbus_set_float(float f, uint16_t *dest);

// int modbus_connect(modbus_t *ctx);
// Integer connect(External);
void js_connect(const v8::FunctionCallbackInfo<v8::Value>& args) {
	modbus_t *ctx = static_cast<modbus_t *>(FROM_EXTERNAL(args[0]));
	
	int ret = modbus_connect(ctx);
	
	args.GetReturnValue().Set(ret);
}

// void modbus_close(modbus_t *ctx);
// Undefined close(External);
void js_close(const v8::FunctionCallbackInfo<v8::Value>& args) {
	modbus_t *ctx = static_cast<modbus_t *>(FROM_EXTERNAL(args[0]));
	
	modbus_close(ctx);
	
	args.GetReturnValue().SetUndefined();
}

// int modbus_flush(modbus_t *ctx);
// Integer flush(External);
void js_flush(const v8::FunctionCallbackInfo<v8::Value>& args) {
	modbus_t *ctx = static_cast<modbus_t *>(FROM_EXTERNAL(args[0]));
	
	int ret = modbus_flush(ctx);
	
	args.GetReturnValue().Set(ret);
}

// int modbus_read_bits(modbus_t *ctx, int addr, int nb, uint8_t *dest);
// Integer read_bits(External, Integer, Integer, Array);
void js_read_bits(const v8::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = v8::Isolate::GetCurrent();
	HandleScope scope(isolate);
	modbus_t *ctx = static_cast<modbus_t *>(FROM_EXTERNAL(args[0]));
	int addr = Local<Integer>::Cast(args[1])->Int32Value();
	int nb = Local<Integer>::Cast(args[2])->Int32Value();
	Local<Array> dest_arr = Local<Array>::Cast(args[3]);
	
	uint8_t dest[nb];
	int ret = modbus_read_bits(ctx, addr, nb, dest);
	
	for (int i = 0; i < nb; i++) dest_arr->Set(i, Number::New(isolate, dest[i]));
	
	args.GetReturnValue().Set(ret);
}

// int modbus_read_input_bits(modbus_t *ctx, int addr, int nb, uint8_t *dest);
// Integer read_input_bits(External, Integer, Integer, Array);
void js_read_input_bits(const v8::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = v8::Isolate::GetCurrent();
	HandleScope scope(isolate);
	modbus_t *ctx = static_cast<modbus_t *>(FROM_EXTERNAL(args[0]));
	int addr = Local<Integer>::Cast(args[1])->Int32Value();
	int nb = Local<Integer>::Cast(args[2])->Int32Value();
	Local<Array> dest_arr = Local<Array>::Cast(args[3]);
	
	uint8_t dest[nb];
	int ret = modbus_read_input_bits(ctx, addr, nb, dest);
	
	for (int i = 0; i < nb; i++) dest_arr->Set(i, Number::New(isolate, dest[i]));
	
	args.GetReturnValue().Set(ret);
}

// int modbus_read_registers(modbus_t *ctx, int addr, int nb, uint16_t *dest);
// Integer read_registers(External, Integer, Integer, Array);
void js_read_registers(const v8::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = v8::Isolate::GetCurrent();
	HandleScope scope(isolate);
	modbus_t *ctx = static_cast<modbus_t *>(FROM_EXTERNAL(args[0]));
	int addr = Local<Integer>::Cast(args[1])->Int32Value();
	int nb = Local<Integer>::Cast(args[2])->Int32Value();
	Local<Array> dest_arr = Local<Array>::Cast(args[3]);
	
	uint16_t dest[nb];
	int ret = modbus_read_registers(ctx, addr, nb, dest);
	
	for (int i = 0; i < nb; i++) dest_arr->Set(i, Number::New(isolate, dest[i]));
	
	args.GetReturnValue().Set(ret);
}

// int modbus_read_input_registers(modbus_t *ctx, int addr, int nb, uint16_t *dest);
// Integer read_input_registers(External, Integer, Integer, Array);
void js_read_input_registers(const v8::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = v8::Isolate::GetCurrent();
	HandleScope scope(isolate);
	modbus_t *ctx = static_cast<modbus_t *>(FROM_EXTERNAL(args[0]));
	int addr = Local<Integer>::Cast(args[1])->Int32Value();
	int nb = Local<Integer>::Cast(args[2])->Int32Value();
	Local<Array> dest_arr = Local<Array>::Cast(args[3]);
	
	uint16_t dest[nb];
	int ret = modbus_read_input_registers(ctx, addr, nb, dest);
	
	for (int i = 0; i < nb; i++) dest_arr->Set(i, Number::New(isolate, dest[i]));
	
	args.GetReturnValue().Set(ret);
}

// int modbus_report_slave_id(modbus_t *ctx, int max_dest, uint8_t *dest);
// Integer report_slave_id(External, Array, Integer);
void js_report_slave_id(const v8::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = v8::Isolate::GetCurrent();
	HandleScope scope(isolate);
	modbus_t *ctx = static_cast<modbus_t *>(FROM_EXTERNAL(args[0]));
	Local<Array> dest_obj = Local<Array>::Cast(args[1]);
	int max_dest = Local<Integer>::Cast(args[2])->Int32Value();
	
	uint8_t dest[REPORT_LEN];
    memset(dest, 0, REPORT_LEN * sizeof(uint8_t));
	
	int ret = modbus_report_slave_id(ctx, max_dest, dest);
	
	if (ret > 0) dest_obj->Set(0, Integer::New(isolate, dest[0])); // Slave ID
	if (ret > 1) dest_obj->Set(1, Integer::New(isolate, dest[1])); // Run Status Indicator
	if (ret > 2) { // Additional data
		for (int i = 2; i < ret; i++) dest_obj->Set(i, Integer::New(isolate, dest[i]));
	}
	
	args.GetReturnValue().Set(ret);
}

// int modbus_write_bit(modbus_t *ctx, int addr, int status);
// Integer write_bit(External, Integer, Integer);
void js_write_bit(const v8::FunctionCallbackInfo<v8::Value>& args) {
	modbus_t *ctx = static_cast<modbus_t *>(FROM_EXTERNAL(args[0]));
	int addr = Local<Integer>::Cast(args[1])->Int32Value();
	int status = Local<Integer>::Cast(args[2])->Int32Value();
	
	int ret = modbus_write_bit(ctx, addr, status);
	
	args.GetReturnValue().Set(ret);
}

// int modbus_write_register(modbus_t *ctx, int addr, int value);
// Integer write_register(External, Integer, Integer);
void js_write_register(const v8::FunctionCallbackInfo<v8::Value>& args) {
	modbus_t *ctx = static_cast<modbus_t *>(FROM_EXTERNAL(args[0]));
	int addr = Local<Integer>::Cast(args[1])->Int32Value();
	int value = Local<Integer>::Cast(args[2])->Int32Value();
	
	int ret = modbus_write_register(ctx, addr, value);
	
	args.GetReturnValue().Set(ret);
}

// int modbus_write_bits(modbus_t *ctx, int addr, int nb, const uint8_t *src);
// Integer write_bits(External, Integer, Integer, Array);
void js_write_bits(const v8::FunctionCallbackInfo<v8::Value>& args) {
	modbus_t *ctx = static_cast<modbus_t *>(FROM_EXTERNAL(args[0]));
	int addr = Local<Integer>::Cast(args[1])->Int32Value();
	int nb = Local<Integer>::Cast(args[2])->Int32Value();
	Local<Array> src_arr = Local<Array>::Cast(args[3]);
	
	uint8_t src[nb];
	for (int i = 0; i < nb; i++) src[i] = src_arr->Get(i)->Uint32Value();
	
	int ret = modbus_write_bits(ctx, addr, nb, src);
	
	args.GetReturnValue().Set(ret);
}

// int modbus_write_registers(modbus_t *ctx, int addr, int nb, const uint16_t *src);
// Integer write_registers(External, Integer, Integer, Array);
void js_write_registers(const v8::FunctionCallbackInfo<v8::Value>& args) {
	modbus_t *ctx = static_cast<modbus_t *>(FROM_EXTERNAL(args[0]));
	int addr = Local<Integer>::Cast(args[1])->Int32Value();
	int nb = Local<Integer>::Cast(args[2])->Int32Value();
	Local<Array> src_arr = Local<Array>::Cast(args[3]);
	
	uint16_t src[nb];
	for (int i = 0; i < nb; i++) src[i] = src_arr->Get(i)->Uint32Value();
	
	int ret = modbus_write_registers(ctx, addr, nb, src);
	
	args.GetReturnValue().Set(ret);
}

// int modbus_write_and_read_registers(modbus_t *ctx, int write_addr, int write_nb, const uint16_t *src, int read_addr, int read_nb, const uint16_t *dest);
// Integer write_and_read_registers(External, Integer, Integer, Array, Integer, Integer, Array);
void js_write_and_read_registers(const v8::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = v8::Isolate::GetCurrent();
	HandleScope scope(isolate);
	modbus_t *ctx = static_cast<modbus_t *>(FROM_EXTERNAL(args[0]));
	int write_addr = Local<Integer>::Cast(args[1])->Int32Value();
	int write_nb = Local<Integer>::Cast(args[2])->Int32Value();
	Local<Array> src_arr = Local<Array>::Cast(args[3]);
	int read_addr = Local<Integer>::Cast(args[4])->Int32Value();
	int read_nb = Local<Integer>::Cast(args[5])->Int32Value();
	Local<Array> dest_arr = Local<Array>::Cast(args[6]);
	
	uint16_t src[write_nb];
	for (int i = 0; i < write_nb; i++) src[i] = src_arr->Get(i)->Uint32Value();
	
	uint16_t dest[read_nb];
	
	int ret = modbus_write_and_read_registers(ctx,
		write_addr, write_nb, src,
		read_addr, read_nb, dest);
	
	for (int i = 0; i < read_nb; i++) dest_arr->Set(i, Number::New(isolate, dest[i]));
	
	args.GetReturnValue().Set(ret);
}

//int modbus_send_raw_request(modbus_t *ctx, uint8_t *raw_req, int raw_req_length);
// Integer send_raw_request(External, Array, Integer);
void js_send_raw_request(const v8::FunctionCallbackInfo<v8::Value>& args) {
	modbus_t *ctx = static_cast<modbus_t *>(FROM_EXTERNAL(args[0]));
	Local<Array> raw_req_arr = Local<Array>::Cast(args[1]);
	int raw_req_length = Local<Integer>::Cast(args[2])->Int32Value();
	
	uint8_t raw_req[raw_req_length];
	for (int i = 0; i < raw_req_length; i++) raw_req[i] = raw_req_arr->Get(i)->Uint32Value();
	
	int ret = modbus_send_raw_request(ctx, raw_req, raw_req_length);
	
	args.GetReturnValue().Set(ret);
}

// int modbus_receive_confirmation(modbus_t *ctx, uint8_t *rsp);
// Integer receive_confirmation(External, Array);
void js_receive_confirmation(const v8::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = v8::Isolate::GetCurrent();
	HandleScope scope(isolate);
	modbus_t *ctx = static_cast<modbus_t *>(FROM_EXTERNAL(args[0]));
	Local<Array> rsp_arr = Local<Array>::Cast(args[1]);
	
	uint8_t rsp[MODBUS_TCP_MAX_ADU_LENGTH];
	memset(rsp, 0, MODBUS_TCP_MAX_ADU_LENGTH * sizeof(uint8_t));
	
	int ret = modbus_receive_confirmation(ctx, rsp);
	
	if (ret > 0) {
		for (int i = 0; i < ret; i++) rsp_arr->Set(i, Number::New(isolate, rsp[i]));
	}
	
	args.GetReturnValue().Set(ret);
}

// int modbus_reply_exception(modbus_t *ctx, const uint8_t *req, unsigned int exception_code);
// Integer reply_exception(External, Array, Integer);
void js_reply_exception(const v8::FunctionCallbackInfo<v8::Value>& args) {
	modbus_t *ctx = static_cast<modbus_t *>(FROM_EXTERNAL(args[0]));
	Local<Array> req_arr = Local<Array>::Cast(args[1]);
	unsigned int exception_code = Local<Integer>::Cast(args[2])->Int32Value();
	
	int req_arr_len = req_arr->InternalFieldCount();
	uint8_t req[req_arr_len];
	for (int i = 0; i < req_arr_len; i++) req[i] = req_arr->Get(i)->Uint32Value();
	
	int ret = modbus_reply_exception(ctx, req, exception_code);
	
	args.GetReturnValue().Set(ret);
}

// modbus_mapping_t *modbus_mapping_new(int nb_bits, int nb_input_bits, int nb_registers, int nb_input_registers);
// External mapping_new(Integer, Integer, Integer, Integer);
void js_mapping_new(const v8::FunctionCallbackInfo<v8::Value>& args) {
	int nb_bits = Local<Integer>::Cast(args[0])->Int32Value();
	int nb_input_bits = Local<Integer>::Cast(args[1])->Int32Value();
	int nb_registers = Local<Integer>::Cast(args[2])->Int32Value();
	int nb_input_registers = Local<Integer>::Cast(args[3])->Int32Value();
	
	modbus_mapping_t *map = modbus_mapping_new(nb_bits, nb_input_bits, nb_registers, nb_input_registers);
	
	if (map == NULL) {
		args.GetReturnValue().SetNull();
	} else {
		args.GetReturnValue().Set(TO_EXTERNAL(map));
	}
}

// void modbus_mapping_free(modbus_mapping_t *mb_mapping);
// Undefined mapping_free(External);
void js_mapping_free(const v8::FunctionCallbackInfo<v8::Value>& args) {
	modbus_mapping_t *map = static_cast<modbus_mapping_t *>(FROM_EXTERNAL(args[0]));
	
	modbus_mapping_free(map);
	
	args.GetReturnValue().SetUndefined();
}

// int modbus_receive(modbus_t *ctx, uint8_t *req);
// Integer receive(External, Array);
void js_receive(const v8::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = v8::Isolate::GetCurrent();
	HandleScope scope(isolate);
	modbus_t *ctx = static_cast<modbus_t *>(FROM_EXTERNAL(args[0]));
	Local<Array> req_arr = Local<Array>::Cast(args[1]);
	
	uint8_t req[MODBUS_TCP_MAX_ADU_LENGTH];
    memset(req, 0, MODBUS_TCP_MAX_ADU_LENGTH * sizeof(uint8_t));
	
	int ret = modbus_receive(ctx, req);
	
	if (ret > 0) {
		for (int i = 0; i < ret; i++) req_arr->Set(i, Number::New(isolate, req[i]));
	}
	
	args.GetReturnValue().Set(ret);
}

// int modbus_reply(modbus_t *ctx, const uint8_t *req, int req_length, modbus_mapping_t *mb_mapping);
// Integer reply(External, Array, Integer, External);
void js_reply(const v8::FunctionCallbackInfo<v8::Value>& args) {
	modbus_t *ctx = static_cast<modbus_t *>(FROM_EXTERNAL(args[0]));
	Local<Array> req_arr = Local<Array>::Cast(args[1]);
	int req_length = Local<Integer>::Cast(args[2])->Int32Value();
	modbus_mapping_t *mb_mapping = static_cast<modbus_mapping_t *>(FROM_EXTERNAL(args[3]));
	
	uint8_t req[req_length];
	for (int i = 0; i < req_length; i++) req[i] = req_arr->Get(i)->Uint32Value();
	
	int ret = modbus_reply(ctx, req, req_length, mb_mapping);
	
	args.GetReturnValue().Set(ret);
}

// const char *modbus_strerror(*int errnum);
// String strerror();
void js_strerror(const v8::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = v8::Isolate::GetCurrent();
	HandleScope scope(isolate);
	const char *ret = modbus_strerror(errno);
	
	args.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, ret));
}

// int modbus_tcp_listen(modbus_t *ctx, int nb_connection);
// Integer tcp_listen(External, Integer);
void js_tcp_listen(const v8::FunctionCallbackInfo<v8::Value>& args) {
	modbus_t *ctx = static_cast<modbus_t *>(FROM_EXTERNAL(args[0]));
	int nb_connection = Local<Integer>::Cast(args[1])->Int32Value();
	
	int ret = modbus_tcp_listen(ctx, nb_connection);
	
	args.GetReturnValue().Set(ret);
}

// int modbus_tcp_accept(modbus_t *ctx, int *socket);
// Integer tcp_accept(External, Integer);
void js_tcp_accept(const v8::FunctionCallbackInfo<v8::Value>& args) {
	modbus_t *ctx = static_cast<modbus_t *>(FROM_EXTERNAL(args[0]));
	int socket = Local<Integer>::Cast(args[1])->Int32Value();
	
	int ret = modbus_tcp_accept(ctx, &socket);
	
	args.GetReturnValue().Set(ret);
}

// int modbus_tcp_pi_listen(modbus_t *ctx, int nb_connection);
// Integer tcp_pi_listen(External, Integer);
void js_tcp_pi_listen(const v8::FunctionCallbackInfo<v8::Value>& args) {
	modbus_t *ctx = static_cast<modbus_t *>(FROM_EXTERNAL(args[0]));
	int nb_connection = Local<Integer>::Cast(args[1])->Int32Value();
	
	int ret = modbus_tcp_pi_listen(ctx, nb_connection);
	
	args.GetReturnValue().Set(ret);
}

// int modbus_tcp_pi_accept(modbus_t *ctx, int *socket);
// Integer tcp_pi_accept(External, Integer);
void js_tcp_pi_accept(const v8::FunctionCallbackInfo<v8::Value>& args) {
	modbus_t *ctx = static_cast<modbus_t *>(FROM_EXTERNAL(args[0]));
	int socket = Local<Integer>::Cast(args[1])->Int32Value();
	
	int ret = modbus_tcp_pi_accept(ctx, &socket);
	
	args.GetReturnValue().Set(ret);
}

// convert modbus_mapping_t* to json object
// Undefined map_to_json(External, Object);
void map_to_json(const v8::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = v8::Isolate::GetCurrent();
	HandleScope scope(isolate);
	modbus_mapping_t *map = static_cast<modbus_mapping_t *>(FROM_EXTERNAL(args[0]));
	Local<Object> jso = Local<Object>::Cast(args[1]);
	
	jso->Set(String::NewFromUtf8(isolate, "nb_bits"), Integer::New(isolate, map->nb_bits));
	jso->Set(String::NewFromUtf8(isolate, "nb_input_bits"), Integer::New(isolate, map->nb_input_bits));
	jso->Set(String::NewFromUtf8(isolate, "nb_input_registers"), Integer::New(isolate, map->nb_input_registers));
	jso->Set(String::NewFromUtf8(isolate, "nb_registers"), Integer::New(isolate, map->nb_registers));
	
	Local<Array> tab_bits = Array::New(isolate);
	for (int i = 0; i < map->nb_bits; i++) {
		tab_bits->Set(i, Integer::New(isolate, map->tab_bits[i]));
	}
	
	Local<Array> tab_input_bits = Array::New(isolate);
	for (int i = 0; i < map->nb_input_bits; i++) {
		tab_input_bits->Set(i, Integer::New(isolate, map->tab_input_bits[i]));
	}
	
	Local<Array> tab_input_registers = Array::New(isolate);
	for (int i = 0; i < map->nb_input_registers; i++) {
		tab_input_registers->Set(i, Integer::New(isolate, map->tab_input_registers[i]));
	}
	
	Local<Array> tab_registers = Array::New(isolate);
	for (int i = 0; i < map->nb_registers; i++) {
		tab_registers->Set(i, Integer::New(isolate, map->tab_registers[i]));
	}
	
	jso->Set(String::NewFromUtf8(isolate, "tab_bits"), tab_bits);
	jso->Set(String::NewFromUtf8(isolate, "tab_input_bits"), tab_input_bits);
	jso->Set(String::NewFromUtf8(isolate, "tab_input_registers"), tab_input_registers);
	jso->Set(String::NewFromUtf8(isolate, "tab_registers"), tab_registers);
	
	args.GetReturnValue().SetUndefined();
}

// convert json object to modbus_mapping_t*
// Undefined json_to_map(Object, External);
void json_to_map(const v8::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = v8::Isolate::GetCurrent();
	HandleScope scope(isolate);
	Local<Object> jso = Local<Object>::Cast(args[0]);
	modbus_mapping_t *map = static_cast<modbus_mapping_t *>(FROM_EXTERNAL(args[1]));
	
	map->nb_bits = jso->Get(String::NewFromUtf8(isolate, "nb_bits"))->Int32Value();
	map->nb_input_bits = jso->Get(String::NewFromUtf8(isolate, "nb_input_bits"))->Int32Value();
	map->nb_input_registers = jso->Get(String::NewFromUtf8(isolate, "nb_input_registers"))->Int32Value();
	map->nb_registers = jso->Get(String::NewFromUtf8(isolate, "nb_registers"))->Int32Value();
	
	Local<Array> tab_bits = Local<Array>::Cast(jso->Get(String::NewFromUtf8(isolate, "tab_bits")));
	for (int i = 0; i < map->nb_bits; i++) {
		map->tab_bits[i] = tab_bits->Get(i)->Int32Value();
	}
	
	Local<Array> tab_input_bits = Local<Array>::Cast(jso->Get(String::NewFromUtf8(isolate, "tab_input_bits")));
	for (int i = 0; i < map->nb_input_bits; i++) {
		map->tab_input_bits[i] = tab_input_bits->Get(i)->Int32Value();
	}
	
	Local<Array> tab_input_registers = Local<Array>::Cast(jso->Get(String::NewFromUtf8(isolate, "tab_input_registers")));
	for (int i = 0; i < map->nb_input_registers; i++) {
		map->tab_input_registers[i] = tab_input_registers->Get(i)->Int32Value();
	}
	
	Local<Array> tab_registers = Local<Array>::Cast(jso->Get(String::NewFromUtf8(isolate, "tab_registers")));
	for (int i = 0; i < map->nb_registers; i++) {
		map->tab_registers[i] = tab_registers->Get(i)->Int32Value();
	}
	
	args.GetReturnValue().SetUndefined();
}

struct tcp_accept_t {
    modbus_t *ctx;
    int socket;
    Persistent<Function> cb;
    int ret;
};

void tcp_accept_w(uv_work_t* req) {
    tcp_accept_t* request = (tcp_accept_t*)req->data;
    request->ret = modbus_tcp_accept(request->ctx, &(request->socket));
}

void tcp_accept_a(uv_work_t* req, int arg) {
    Isolate* isolate = v8::Isolate::GetCurrent();
    HandleScope scope(isolate);
    tcp_accept_t* request = (tcp_accept_t*)req->data;
    delete req;
	
    Handle<Value> argv[1];
    argv[0] = Integer::New(isolate, request->ret);

    v8::Local<v8::Function> lf = v8::Local<v8::Function>::New(isolate,request->cb);
    lf->Call(Null(isolate), 1, argv);
	
    request->cb.Reset();
    delete request;
}

// Undefined tcp_accept_async(External, Integer, Function);
// callback function - Function(Integer);
void tcp_accept_async(const v8::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = v8::Isolate::GetCurrent();
	HandleScope scope(isolate);
	modbus_t *ctx = static_cast<modbus_t *>(FROM_EXTERNAL(args[0]));
	int socket = Local<Integer>::Cast(args[1])->Int32Value();
	Local<Function> cb = Local<Function>::Cast(args[2]);
	
	tcp_accept_t* request = new tcp_accept_t;
	request->ctx = ctx;
	request->socket = socket;
	request->cb.Reset(isolate, cb);
	
	uv_work_t* req = new uv_work_t();
	req->data = request;
	
	uv_queue_work(uv_default_loop(), req, tcp_accept_w, tcp_accept_a);
	
	args.GetReturnValue().SetUndefined();
}

struct receive_t {
    modbus_t *ctx;
    uint8_t *req;
    int len;
    Persistent<Function> cb;
};

void receive_w(uv_work_t* req) {
    receive_t* request = (receive_t*)req->data;
    request->len = modbus_receive(request->ctx, request->req);
}

void receive_a(uv_work_t* req, int arg) {
    Isolate* isolate = v8::Isolate::GetCurrent();
    HandleScope scope(isolate);
    receive_t* request = (receive_t*)req->data;
    delete req;
	
	int len = request->len;
	
	Local<Array> req_arr = Array::New(isolate);
	if (len > 0) {
		for (int i = 0; i < len; i++) req_arr->Set(i, Number::New(isolate, request->req[i]));
	}
	
	delete request->req;
	
    Handle<Value> argv[2];
    argv[0] = req_arr;
    argv[1] = Integer::New(isolate, len);
    
    v8::Local<v8::Function> lf = v8::Local<v8::Function>::New(isolate,request->cb);
    lf->Call(Null(isolate), 2, argv);
	
    request->cb.Reset();
    delete request;
}

// Undefined receive_async(External, Function);
// callback function - Function(Array, Integer);
void receive_async(const v8::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = v8::Isolate::GetCurrent();
	HandleScope scope(isolate);
	modbus_t *ctx = static_cast<modbus_t *>(FROM_EXTERNAL(args[0]));
	Local<Function> cb = Local<Function>::Cast(args[1]);
	
	uint8_t *mbreq = new uint8_t[MODBUS_TCP_MAX_ADU_LENGTH];
	memset(mbreq, 0, MODBUS_TCP_MAX_ADU_LENGTH * sizeof(uint8_t));
	
	receive_t* request = new receive_t;
	request->ctx = ctx;
	request->req = mbreq;
	request->len = 0;
	request->cb.Reset(isolate, cb);
	
	uv_work_t* req = new uv_work_t();
	req->data = request;
	
	uv_queue_work(uv_default_loop(), req, receive_w, receive_a);
	
	args.GetReturnValue().SetUndefined();
}

struct connect_t {
    modbus_t *ctx;
    Persistent<Function> cb;
    int ret;
};

void connect_w(uv_work_t* req) {
    connect_t* request = (connect_t*)req->data;
    request->ret = modbus_connect(request->ctx);
}

void connect_a(uv_work_t* req, int arg) {
    Isolate* isolate = v8::Isolate::GetCurrent();
    HandleScope scope(isolate);
    connect_t* request = (connect_t*)req->data;
    delete req;
	
    Handle<Value> argv[1];
    argv[0] = Integer::New(isolate, request->ret);
    
    v8::Local<v8::Function> lf = v8::Local<v8::Function>::New(isolate,request->cb);
    lf->Call(Null(isolate), 1, argv);
	
    request->cb.Reset();
    delete request;
}

// Undefined connect_async(External, Function);
// callback function - Function(Integer);
void connect_async(const v8::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = v8::Isolate::GetCurrent();
	HandleScope scope(isolate);
	modbus_t *ctx = static_cast<modbus_t *>(FROM_EXTERNAL(args[0]));
	Local<Function> cb = Local<Function>::Cast(args[1]);
	
	connect_t* request = new connect_t;
	request->ctx = ctx;
	request->cb.Reset(isolate, cb);
	request->ret = 0;
	
	uv_work_t* req = new uv_work_t();
	req->data = request;
	
	uv_queue_work(uv_default_loop(), req, connect_w, connect_a);
	
	args.GetReturnValue().SetUndefined();
}

// закрыть из треда
// Undefined close(External);
void close_mt(const v8::FunctionCallbackInfo<v8::Value>& args) {
	modbus_t *ctx = static_cast<modbus_t *>(FROM_EXTERNAL(args[0]));
	
	modbus_close(ctx);
	
	args.GetReturnValue().SetUndefined();
}

// Decode HEX value to a float or double
void hex_decode(const v8::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = v8::Isolate::GetCurrent();
	HandleScope scope(isolate);
	int nArgs = args.Length();

	if (nArgs != 2 && nArgs != 4) {
		isolate->ThrowException(Exception::TypeError(String::NewFromUtf8(isolate, "Need at least 2 or 4 16-bit numbers")));
		return;
	}

	uint16_t input[nArgs];
	for (int i = 0; i < nArgs; i++) {
		input[i] = (uint16_t) args[i]->ToInteger()->Value();
	}

	if (nArgs == 2) {
		uint32_t raw_value = (((uint32_t) input[0]) << 16) + input[1];
		float output;
		memcpy(&output, &raw_value, sizeof(float));
		args.GetReturnValue().Set(output);
	} else {
		uint64_t raw_value = (((uint64_t) input[0]) << 48) + ((uint64_t) input[1] << 32) + ((uint32_t) input[2] << 16) + input[3];
		double output;
		memcpy(&output, &raw_value, sizeof(double));

		args.GetReturnValue().Set(output);
	}
}

extern "C" void init (Handle<Object> target) {
	Isolate* isolate = target->GetIsolate();
	
	// constants
	target->Set(String::NewFromUtf8(isolate, "LIBMODBUS_VERSION_MAJOR"), Number::New(isolate, LIBMODBUS_VERSION_MAJOR));
	target->Set(String::NewFromUtf8(isolate, "LIBMODBUS_VERSION_MINOR"), Number::New(isolate, LIBMODBUS_VERSION_MINOR));
	target->Set(String::NewFromUtf8(isolate, "LIBMODBUS_VERSION_MICRO"), Number::New(isolate, LIBMODBUS_VERSION_MICRO));
	target->Set(String::NewFromUtf8(isolate, "LIBMODBUS_VERSION_STRING"), String::NewFromUtf8(isolate, LIBMODBUS_VERSION_STRING));
	//target->Set(String::NewFromUtf8(isolate, "LIBMODBUS_VERSION_HEX"), Number::New(isolate, LIBMODBUS_VERSION_HEX)); bug in header
	
	target->Set(String::NewFromUtf8(isolate, "FALSE"), Number::New(isolate, FALSE));
	target->Set(String::NewFromUtf8(isolate, "TRUE"), Number::New(isolate, TRUE));
	
	target->Set(String::NewFromUtf8(isolate, "OFF"), Number::New(isolate, OFF));
	target->Set(String::NewFromUtf8(isolate, "ON"), Number::New(isolate, ON));
	
	target->Set(String::NewFromUtf8(isolate, "MODBUS_BROADCAST_ADDRESS"), Number::New(isolate, MODBUS_BROADCAST_ADDRESS));
	
	target->Set(String::NewFromUtf8(isolate, "MODBUS_MAX_READ_BITS"), Number::New(isolate, MODBUS_MAX_READ_BITS));
	target->Set(String::NewFromUtf8(isolate, "MODBUS_MAX_WRITE_BITS"), Number::New(isolate, MODBUS_MAX_WRITE_BITS));
	
	target->Set(String::NewFromUtf8(isolate, "MODBUS_MAX_READ_REGISTERS"), Number::New(isolate, MODBUS_MAX_READ_REGISTERS));
	target->Set(String::NewFromUtf8(isolate, "MODBUS_MAX_WRITE_REGISTERS"), Number::New(isolate, MODBUS_MAX_WRITE_REGISTERS));
	target->Set(String::NewFromUtf8(isolate, "MODBUS_MAX_WR_WRITE_REGISTERS"), Number::New(isolate, MODBUS_MAX_WR_WRITE_REGISTERS));
	
	target->Set(String::NewFromUtf8(isolate, "MODBUS_ENOBASE"), Number::New(isolate, MODBUS_ENOBASE));
	
	target->Set(String::NewFromUtf8(isolate, "MODBUS_EXCEPTION_ILLEGAL_FUNCTION"), Number::New(isolate, MODBUS_EXCEPTION_ILLEGAL_FUNCTION));
	target->Set(String::NewFromUtf8(isolate, "MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS"), Number::New(isolate, MODBUS_EXCEPTION_ILLEGAL_DATA_ADDRESS));
	target->Set(String::NewFromUtf8(isolate, "MODBUS_EXCEPTION_ILLEGAL_DATA_VALUE"), Number::New(isolate, MODBUS_EXCEPTION_ILLEGAL_DATA_VALUE));
	target->Set(String::NewFromUtf8(isolate, "MODBUS_EXCEPTION_SLAVE_OR_SERVER_FAILURE"), Number::New(isolate, MODBUS_EXCEPTION_SLAVE_OR_SERVER_FAILURE));
	target->Set(String::NewFromUtf8(isolate, "MODBUS_EXCEPTION_ACKNOWLEDGE"), Number::New(isolate, MODBUS_EXCEPTION_ACKNOWLEDGE));
	target->Set(String::NewFromUtf8(isolate, "MODBUS_EXCEPTION_SLAVE_OR_SERVER_BUSY"), Number::New(isolate, MODBUS_EXCEPTION_SLAVE_OR_SERVER_BUSY));
	target->Set(String::NewFromUtf8(isolate, "MODBUS_EXCEPTION_NEGATIVE_ACKNOWLEDGE"), Number::New(isolate, MODBUS_EXCEPTION_NEGATIVE_ACKNOWLEDGE));
	target->Set(String::NewFromUtf8(isolate, "MODBUS_EXCEPTION_MEMORY_PARITY"), Number::New(isolate, MODBUS_EXCEPTION_MEMORY_PARITY));
	target->Set(String::NewFromUtf8(isolate, "MODBUS_EXCEPTION_NOT_DEFINED"), Number::New(isolate, MODBUS_EXCEPTION_NOT_DEFINED));
	target->Set(String::NewFromUtf8(isolate, "MODBUS_EXCEPTION_GATEWAY_PATH"), Number::New(isolate, MODBUS_EXCEPTION_GATEWAY_PATH));
	target->Set(String::NewFromUtf8(isolate, "MODBUS_EXCEPTION_GATEWAY_TARGET"), Number::New(isolate, MODBUS_EXCEPTION_GATEWAY_TARGET));
	target->Set(String::NewFromUtf8(isolate, "MODBUS_EXCEPTION_MAX"), Number::New(isolate, MODBUS_EXCEPTION_MAX));
	
	target->Set(String::NewFromUtf8(isolate, "EMBXILFUN"), Number::New(isolate, EMBXILFUN));
	target->Set(String::NewFromUtf8(isolate, "EMBXILADD"), Number::New(isolate, EMBXILADD));
	target->Set(String::NewFromUtf8(isolate, "EMBXILVAL"), Number::New(isolate, EMBXILVAL));
	target->Set(String::NewFromUtf8(isolate, "EMBXSFAIL"), Number::New(isolate, EMBXSFAIL));
	target->Set(String::NewFromUtf8(isolate, "EMBXACK"), Number::New(isolate, EMBXACK));
	target->Set(String::NewFromUtf8(isolate, "EMBXSBUSY"), Number::New(isolate, EMBXSBUSY));
	target->Set(String::NewFromUtf8(isolate, "EMBXNACK"), Number::New(isolate, EMBXNACK));
	target->Set(String::NewFromUtf8(isolate, "EMBXMEMPAR"), Number::New(isolate, EMBXMEMPAR));
	target->Set(String::NewFromUtf8(isolate, "EMBXGPATH"), Number::New(isolate, EMBXGPATH));
	target->Set(String::NewFromUtf8(isolate, "EMBXGTAR"), Number::New(isolate, EMBXGTAR));
	
	target->Set(String::NewFromUtf8(isolate, "EMBBADCRC"), Number::New(isolate, EMBBADCRC));
	target->Set(String::NewFromUtf8(isolate, "EMBBADDATA"), Number::New(isolate, EMBBADDATA));
	target->Set(String::NewFromUtf8(isolate, "EMBBADEXC"), Number::New(isolate, EMBBADEXC));
	target->Set(String::NewFromUtf8(isolate, "EMBUNKEXC"), Number::New(isolate, EMBUNKEXC));
	target->Set(String::NewFromUtf8(isolate, "EMBMDATA"), Number::New(isolate, EMBMDATA));
	
	target->Set(String::NewFromUtf8(isolate, "MODBUS_ERROR_RECOVERY_NONE"), Number::New(isolate, MODBUS_ERROR_RECOVERY_NONE));
	target->Set(String::NewFromUtf8(isolate, "MODBUS_ERROR_RECOVERY_LINK"), Number::New(isolate, MODBUS_ERROR_RECOVERY_LINK));
	target->Set(String::NewFromUtf8(isolate, "MODBUS_ERROR_RECOVERY_PROTOCOL"), Number::New(isolate, MODBUS_ERROR_RECOVERY_PROTOCOL));
	
	target->Set(String::NewFromUtf8(isolate, "MODBUS_RTU_MAX_ADU_LENGTH"), Number::New(isolate, MODBUS_RTU_MAX_ADU_LENGTH));
	target->Set(String::NewFromUtf8(isolate, "MODBUS_RTU_RS232"), Number::New(isolate, MODBUS_RTU_RS232));
	target->Set(String::NewFromUtf8(isolate, "MODBUS_RTU_RS485"), Number::New(isolate, MODBUS_RTU_RS485));
	
	target->Set(String::NewFromUtf8(isolate, "MODBUS_TCP_DEFAULT_PORT"), Number::New(isolate, MODBUS_TCP_DEFAULT_PORT));
	target->Set(String::NewFromUtf8(isolate, "MODBUS_TCP_SLAVE"), Number::New(isolate, MODBUS_TCP_SLAVE));
	target->Set(String::NewFromUtf8(isolate, "MODBUS_TCP_MAX_ADU_LENGTH"), Number::New(isolate, MODBUS_TCP_MAX_ADU_LENGTH));
	
	// functions
	target->Set(String::NewFromUtf8(isolate, "new_rtu"), FunctionTemplate::New(isolate, js_new_rtu)->GetFunction());
	target->Set(String::NewFromUtf8(isolate, "rtu_get_serial_mode"), FunctionTemplate::New(isolate, js_rtu_get_serial_mode)->GetFunction());
	target->Set(String::NewFromUtf8(isolate, "rtu_set_serial_mode"), FunctionTemplate::New(isolate, js_rtu_set_serial_mode)->GetFunction());
	//target->Set(String::NewFromUtf8(isolate, "rtu_get_rts"), FunctionTemplate::New(isolate, js_rtu_get_rts)->GetFunction());
	//target->Set(String::NewFromUtf8(isolate, "rtu_set_rts"), FunctionTemplate::New(isolate, js_rtu_set_rts)->GetFunction());
	
	target->Set(String::NewFromUtf8(isolate, "new_tcp"), FunctionTemplate::New(isolate, js_new_tcp)->GetFunction());
	target->Set(String::NewFromUtf8(isolate, "new_tcp_pi"), FunctionTemplate::New(isolate, js_new_tcp_pi)->GetFunction());
	
	target->Set(String::NewFromUtf8(isolate, "free"), FunctionTemplate::New(isolate, js_free)->GetFunction());
	
	target->Set(String::NewFromUtf8(isolate, "get_byte_timeout"), FunctionTemplate::New(isolate, js_get_byte_timeout)->GetFunction());
	target->Set(String::NewFromUtf8(isolate, "set_byte_timeout"), FunctionTemplate::New(isolate, js_set_byte_timeout)->GetFunction());
	target->Set(String::NewFromUtf8(isolate, "set_debug"), FunctionTemplate::New(isolate, js_set_debug)->GetFunction());
	target->Set(String::NewFromUtf8(isolate, "set_error_recovery"), FunctionTemplate::New(isolate, js_set_error_recovery)->GetFunction());
	target->Set(String::NewFromUtf8(isolate, "get_header_length"), FunctionTemplate::New(isolate, js_get_header_length)->GetFunction());
	target->Set(String::NewFromUtf8(isolate, "get_response_timeout"), FunctionTemplate::New(isolate, js_get_response_timeout)->GetFunction());
	target->Set(String::NewFromUtf8(isolate, "set_response_timeout"), FunctionTemplate::New(isolate, js_set_response_timeout)->GetFunction());
	target->Set(String::NewFromUtf8(isolate, "set_slave"), FunctionTemplate::New(isolate, js_set_slave)->GetFunction());
	target->Set(String::NewFromUtf8(isolate, "set_socket"), FunctionTemplate::New(isolate, js_set_socket)->GetFunction());
	target->Set(String::NewFromUtf8(isolate, "get_socket"), FunctionTemplate::New(isolate, js_get_socket)->GetFunction());
	
	target->Set(String::NewFromUtf8(isolate, "connect"), FunctionTemplate::New(isolate, js_connect)->GetFunction());
	target->Set(String::NewFromUtf8(isolate, "close"), FunctionTemplate::New(isolate, js_close)->GetFunction());
	target->Set(String::NewFromUtf8(isolate, "flush"), FunctionTemplate::New(isolate, js_flush)->GetFunction());
	
	target->Set(String::NewFromUtf8(isolate, "read_bits"), FunctionTemplate::New(isolate, js_read_bits)->GetFunction());
	target->Set(String::NewFromUtf8(isolate, "read_input_bits"), FunctionTemplate::New(isolate, js_read_input_bits)->GetFunction());
	target->Set(String::NewFromUtf8(isolate, "read_registers"), FunctionTemplate::New(isolate, js_read_registers)->GetFunction());
	target->Set(String::NewFromUtf8(isolate, "read_input_registers"), FunctionTemplate::New(isolate, js_read_input_registers)->GetFunction());
	target->Set(String::NewFromUtf8(isolate, "report_slave_id"), FunctionTemplate::New(isolate, js_report_slave_id)->GetFunction());
	target->Set(String::NewFromUtf8(isolate, "write_bit"), FunctionTemplate::New(isolate, js_write_bit)->GetFunction());
	target->Set(String::NewFromUtf8(isolate, "write_register"), FunctionTemplate::New(isolate, js_write_register)->GetFunction());
	target->Set(String::NewFromUtf8(isolate, "write_bits"), FunctionTemplate::New(isolate, js_write_bits)->GetFunction());
	target->Set(String::NewFromUtf8(isolate, "write_registers"), FunctionTemplate::New(isolate, js_write_registers)->GetFunction());
	target->Set(String::NewFromUtf8(isolate, "write_and_read_registers"), FunctionTemplate::New(isolate, js_write_and_read_registers)->GetFunction());
	target->Set(String::NewFromUtf8(isolate, "send_raw_request"), FunctionTemplate::New(isolate, js_send_raw_request)->GetFunction());
	target->Set(String::NewFromUtf8(isolate, "receive_confirmation"), FunctionTemplate::New(isolate, js_receive_confirmation)->GetFunction());
	target->Set(String::NewFromUtf8(isolate, "reply_exception"), FunctionTemplate::New(isolate, js_reply_exception)->GetFunction());
	
	target->Set(String::NewFromUtf8(isolate, "mapping_new"), FunctionTemplate::New(isolate, js_mapping_new)->GetFunction());
	target->Set(String::NewFromUtf8(isolate, "mapping_free"), FunctionTemplate::New(isolate, js_mapping_free)->GetFunction());
	target->Set(String::NewFromUtf8(isolate, "receive"), FunctionTemplate::New(isolate, js_receive)->GetFunction());
	target->Set(String::NewFromUtf8(isolate, "reply"), FunctionTemplate::New(isolate, js_reply)->GetFunction());
	
	target->Set(String::NewFromUtf8(isolate, "strerror"), FunctionTemplate::New(isolate, js_strerror)->GetFunction());
	
	target->Set(String::NewFromUtf8(isolate, "tcp_listen"), FunctionTemplate::New(isolate, js_tcp_listen)->GetFunction());
	target->Set(String::NewFromUtf8(isolate, "tcp_accept"), FunctionTemplate::New(isolate, js_tcp_accept)->GetFunction());
	target->Set(String::NewFromUtf8(isolate, "tcp_pi_listen"), FunctionTemplate::New(isolate, js_tcp_pi_listen)->GetFunction());
	target->Set(String::NewFromUtf8(isolate, "tcp_pi_accept"), FunctionTemplate::New(isolate, js_tcp_pi_accept)->GetFunction());
	
	// my functions
	target->Set(String::NewFromUtf8(isolate, "map_to_json"), FunctionTemplate::New(isolate, map_to_json)->GetFunction());
	target->Set(String::NewFromUtf8(isolate, "json_to_map"), FunctionTemplate::New(isolate, json_to_map)->GetFunction());
	
	target->Set(String::NewFromUtf8(isolate, "tcp_accept_async"), FunctionTemplate::New(isolate, tcp_accept_async)->GetFunction());
	target->Set(String::NewFromUtf8(isolate, "receive_async"), FunctionTemplate::New(isolate, receive_async)->GetFunction());
	target->Set(String::NewFromUtf8(isolate, "connect_async"), FunctionTemplate::New(isolate, connect_async)->GetFunction());
	target->Set(String::NewFromUtf8(isolate, "close_mt"), FunctionTemplate::New(isolate, close_mt)->GetFunction());

	// HEX Decoding stuff
	target->Set(String::NewFromUtf8(isolate, "hex_decode", v8::String::kInternalizedString), FunctionTemplate::New(isolate, hex_decode)->GetFunction());
}

void Initialize (Handle<Object> exports);
NODE_MODULE(modbus_binding, init)

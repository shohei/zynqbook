
#ifndef __LAYER_H__
#define __LAYER_H__
//----------------------------------------------------------------

class Layer {
public:
	Layer(int a_layer_no, void *a_logicvc_regs_base, void *a_phy_addr, uint32_t a_size, uint32_t w, uint32_t h, uint32_t a_stride_bytes, uint32_t a_buffer_offset_bytes, uint32_t a_bytes_per_pixel) :
		layer_no(a_layer_no), phy_addr(a_phy_addr), size(a_size), mem_addr(0), width(w), height(h), stride_bytes(a_stride_bytes), buffer_offset_bytes(a_buffer_offset_bytes), bytes_per_pixel(a_bytes_per_pixel)
	{
		off_t layer_regs_offset_off_t;

		layer_regs_offset_off_t = (off_t) a_logicvc_regs_base;
		layer_regs_offset_off_t += ( layer_no ) * 0x80 +  0x100;
		layer_regs_base = (void *)layer_regs_offset_off_t;
	}

	void init() {
		mem_addr =  map_dev_mem((off_t)phy_addr, size);
	}

	int layer_no;
	void *layer_regs_base;
	void *phy_addr;
	uint32_t size;
	void *mem_addr;
	uint32_t width;
	uint32_t height;
	uint32_t stride_bytes;
	uint32_t buffer_offset_bytes;
	uint32_t bytes_per_pixel;

	void layer_enable(bool enable_flag) {
		bit_write_reg32(layer_regs_base, 0x38, 1, enable_flag?1:0);
	}
	void select_buffer(uint32_t buffer_no) {
		uint32_t value;

	        value = buffer_no & 0x3;
		value <<= (layer_no * 2);
		value |= (0x400 << layer_no);

		write_reg32(logiCVC_ML_regs, 0x58, value);
	}
	uint32_t get_buffer_no() {
	        uint32_t value;
		value = read_reg32(logiCVC_ML_regs, 0x58);
		return (value >> (layer_no * 2)) & 0x3;
	}

	bool is_buffer_switched()
	{
	        return (read_reg32(logiCVC_ML_regs, 0x68) & (1 << layer_no));
	}

	void clear_buffer_switched() {
	        write_reg32(logiCVC_ML_regs, 0x68, 1 << layer_no);
	}

	void *calc_addr(int x, int y) {
		off_t addr_off_t;
		addr_off_t = (off_t)phy_addr;
		addr_off_t += y * stride_bytes;
		addr_off_t += x * bytes_per_pixel;
		return (void *)addr_off_t;
	}

	static Layer *layer[5];

	void change_buffer(uint32_t layer_no, uint32_t buffer_no) {
		Layer *l;
		l = layer[layer_no];
		uint32_t current_buffer_no;
		current_buffer_no = l->get_buffer_no();
		if ( buffer_no == current_buffer_no ) {
			return;
		}
		l->select_buffer(buffer_no);
		while(! l->is_buffer_switched()) {
		}
		l->clear_buffer_switched();
	}
}; // Layer

//----------------------------------------------------------------
#endif /* __LAYER_H__ */

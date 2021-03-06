#ifndef HDF5HPP
#define HDF5HPP

#include <H5Cpp.h>
#include <string>
#include <vector>
#include <map>
#include <memory>


namespace think { namespace hdf5 {
  using namespace H5;
  using namespace std;

  struct Access {
    enum EEnum {
      excl = 1 << 0,
      trunc = 1 << 1,
      rdonly = 1 << 2,
      rdrw = 1 << 3,
      debug = 1 << 4,
      create = 1 << 5,
    };
  };

  struct EObjType {
    enum EEnum {
      unknown = -1,
      group,
      dataset,
      datatype,
      file,
    };
  };

  class abstract_ds
  {
  public:
    enum EDType {
      dt_no_class = -1,
      dt_integer = 0,
      dt_float = 1,
      dt_time = 2,
      dt_string = 3,
      dt_bitfield = 4,
      dt_opaque = 5,
      dt_compound = 6,
      dt_reference = 7,
      dt_enum = 8,
      dt_vlen = 9,
      dt_array = 10,
    };
    virtual ~abstract_ds() {}
    virtual EDType get_type_class() const = 0;
    virtual size_t get_in_mem_data_size() const = 0;
  };

  class attribute : public abstract_ds
  {
  public:
    virtual ~attribute(){}
    virtual string name() const = 0;
    virtual void read(void* buf, size_t buf_size) const = 0;
  };

  typedef shared_ptr<attribute> shared_attr_ptr;
  typedef vector<shared_attr_ptr> shared_attr_ptr_list;

  class location
  {
  public:
    virtual ~location(){}
    virtual size_t get_attribute_count() const { return 0; }
    virtual attribute* get_attribute(size_t idx) { return NULL; }
  };
  class dataset;
  class object_registry;
  class object;
  typedef vector<object*> obj_ptr_list;
  typedef shared_ptr<object> shared_obj_ptr;
  typedef vector<shared_obj_ptr> shared_obj_ptr_list;

  class object_registry
  {
  public:
    virtual ~object_registry(){}
    virtual object* dereference( int obj_id, long file_offset ) = 0;
  };

  class object : public location
  {
  public:
    virtual ~object() {}
    virtual EObjType::EEnum type() const = 0;
    virtual string name() const  = 0;
    virtual size_t child_count() const { return 0; }
    virtual object* get_child( size_t idx ) { return NULL; }
    virtual dataset* to_dataset() { return NULL; }
    virtual object_registry& registry() = 0;
    virtual int obj_id () const = 0;
  };

  class dataset : public object, public abstract_ds
  {
  public:
    enum EEnum {
      ds_no_class = -1,
      ds_scalar = 0,
      ds_simple = 1,
      ds_null = 2,
    };
    virtual ~dataset(){}
    virtual bool is_simple() const = 0;
    virtual size_t ndims() const = 0;
    //dims are hsize_t so use the library function call to find the sizeof_hsize_t
    //dims are ndims * sizeof(hsize_t)
    virtual void get_dims( void* dims ) const = 0;
    virtual void read( void* buf, size_t buf_size) const = 0;
    virtual bool is_variable_len_string() const = 0;
    virtual size_t string_column_size() const = 0;
    virtual size_t string_size() const = 0;
    //Buf is an array of pointers max(1, n-dims)
    virtual void read_variable_string(void* buf, size_t buf_size ) = 0;
    virtual void release_variable_string( void* buf ) = 0;
    virtual void read_string(void* buf) = 0;
  };

}}

#endif

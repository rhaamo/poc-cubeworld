/* Various compat things */

#if defined(__linux__) || defined(__APPLE__)
typedef char TCHAR;

#if 0 //(__GNUC__ <= 4 && __GNUC_MINOR__ < 6)
#warning Using nullptr hack for GCC <4.6
/* Workaround for `nullptr' with GCC < 4.6
 * http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2007/n2431.pdf
 * http://stackoverflow.com/questions/2419800/can-nullptr-be-emulated-in-gcc
 */
const                        // this is a const object...
class {
public:
  template<class T>          // convertible to any type
    operator T*() const      // of null non-member
    { return 0; }            // pointer...
  template<class C, class T> // or any type of null
    operator T C::*() const  // member pointer...
    { return 0; }
private:
  void operator&() const;    // whose address can't be taken
} nullptr = {};              // and whose name is nullptr
#endif

#endif

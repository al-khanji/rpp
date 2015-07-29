TEMPLATE = aux

headers.path = $$PREFIX/include
headers.files = \
    rpp.hpp \
    rpp_utils.hpp \
    rpp_emitter.hpp

INSTALLS += headers

# -*- coding: cp1251 -*-

from struct import unpack
from xml.sax.saxutils import XMLGenerator

def mxl2xml(fn):

#	originally was
#    fi = file(fn,"rb")
#    fo = file(fn+".xml","w")
    fi = open(fn,"rb")
    fo = open(fn+".xml","w")

    xml = XMLGenerator(fo, "cp1251")
    xml.startDocument()
    __table(fi, xml)
    xml.endDocument()
    fo.close()
    fi.close()

def __write_element(xml,name,value):
    xml.startElement(name,{})
    xml.characters(value)
    xml.endElement(name)
    
def __table(fi, xml):
    # root - table
    buf = fi.read(0x37)
    xml.startElement("table",{})
    __fonts(fi, xml)
    print "_unk1", [fi.read(4)]
    __page_hdrs(fi, xml)
    __cols(fi, xml)
    __rows(fi, xml)
    __objs(fi, xml)
    __merge(fi, xml)
    __sects(fi, xml, "v_sects", "v_sect")
    __sects(fi, xml, "h_sects", "h_sect")
    __pagefeeds(fi, xml, "PageFeedsVert", "PageFeedVert")
    __pagefeeds(fi, xml, "PageFeedsHor", "PageFeedHor")
    __names(fi, xml)
    xml.endElement("table")
    print "_rest", [fi.read()]

def __fonts(fi, xml):
    # fonts
    # numbs
    size1, = unpack("=H", fi.read(2))
    if size1:
        buf = fi.read(size1*4)
        numbs = unpack("="+"i"*size1, buf)
    # props
    size2, = unpack("=H", fi.read(2))
    if size2:
        if size1 != size2:
            raise SyntaxError
        # xml
        xml.startElement("fonts",{"size":str(size2)})
        for i in xrange(size2):
            # xml
            xml.startElement("font",{"id":str(numbs[i])})
            __font(fi, xml)
            xml.endElement("font")
        xml.endElement("fonts")

def __page_hdrs(fi, xml):
    xml.startElement("page_hdr_top",{})
    __cell(fi, xml, 0, "BeginFlag", "BeginNumber")
    xml.endElement("page_hdr_top")
    xml.startElement("page_hdr_bottom",{})
    __cell(fi, xml, 0, "BeginFlag", "BeginNumber")
    xml.endElement("page_hdr_bottom")

def __font(fi, xml):
    buf = fi.read(60)
    name = ""
    for i in buf[0x1c:]:
        if i=="\x00":
            break
        name += i
    __write_element(xml,"name",name)
    #xml.characters(str([buf]))
   
def __cols(fi, xml):
    size1, = unpack("=H", fi.read(2))
    if size1:
        buf = fi.read(size1*4)
        numbs = unpack("="+"i"*size1, buf)

    size2, = unpack("=H", fi.read(2))
    if size2:
        if size1 != size2:
            raise SyntaxError
        xml.startElement("cols",{"size":str(size2)})
        for i in xrange(size2):
            # xml
            xml.startElement("col",{"id":str(numbs[i])})
            __cell(fi, xml, 0, "ПоложНаСтраницеГориз", "ColumnWidth")
            #__col(fi, xml)
            xml.endElement("col")
        xml.endElement("cols")

##def __col(fi, xml):
##    buf = fi.read(30)
##    flag, = unpack("=B",buf[0x01])
##    if flag & 0x08:
##        width, = unpack("=H",buf[0x06:0x08])
##        __write_element(xml,"ColumnWidth",str(width))
##    #xml.characters(str([buf]))
    
def __rows(fi, xml):
    size1, = unpack("=H", fi.read(2))
    if size1:
        buf = fi.read(size1*4)
        numbs = unpack("="+"i"*size1, buf)

    size2, = unpack("=H", fi.read(2))
    if size2:
        if size1 != size2:
            raise SyntaxError
        xml.startElement("rows",{"size":str(size2)})
        for i in xrange(size2):
            # xml
            xml.startElement("row",{"id":str(numbs[i])})
            #__row(fi, xml)
            __cell(fi, xml, 0, "RowHeight", "ПоложНаСтраницеВерт")
            __cells(fi, xml)
            xml.endElement("row")
        xml.endElement("rows")

##def __row(fi, xml):
##    buf = fi.read(30)
##    flag, = unpack("=B",buf[0x01])
##    if flag & 0x04:
##        height, = unpack("=H",buf[0x04:0x06])
##        __write_element(xml,"RowHeight",str(height))
##    #xml.characters(str([buf]))

def __cells(fi, xml):
    size1, = unpack("=H", fi.read(2))
    if size1:
        buf = fi.read(size1*4)
        numbs = unpack("="+"i"*size1, buf)

    size2, = unpack("=H", fi.read(2))
    if size2:
        if size1 != size2:
            raise SyntaxError
        xml.startElement("cols",{"size":str(size2)})
        for i in xrange(size2):
            # xml
            __cell(fi, xml, numbs[i])
        xml.endElement("cols")

def __write_prop(xml, buf, addr_f, flag, addr_v, name):
    flag_, = unpack("=B",buf[addr_f])
    if flag_ & flag:
        value, = unpack("=B", buf[addr_v])
        __write_element(xml,name,str(value))
        
def __cell(fi, xml, id, share1="Share1", share2="Share2"):
    xml.startElement("cell",{"id":str(id)})
    buf = fi.read(30)
    __write_prop(xml, buf, 0x00, 0x01, 0x08, "Font")
    __write_prop(xml, buf, 0x00, 0x02, 0x0a, "FontSize")
    __write_prop(xml, buf, 0x00, 0x04, 0x0c, "Bold")
    __write_prop(xml, buf, 0x00, 0x08, 0x0d, "Italic")
    __write_prop(xml, buf, 0x00, 0x10, 0x0e, "Underline")
    __write_prop(xml, buf, 0x01, 0x10, 0x0f, "HorizontalAlign")
    __write_prop(xml, buf, 0x01, 0x20, 0x10, "VerticalAlign")
    __write_prop(xml, buf, 0x02, 0x01, 0x11, "Узор")
    __write_prop(xml, buf, 0x00, 0x20, 0x12, "LeftBorder")
    __write_prop(xml, buf, 0x00, 0x40, 0x13, "TopBorder")
    __write_prop(xml, buf, 0x00, 0x80, 0x14, "RightBorder")
    __write_prop(xml, buf, 0x01, 0x01, 0x15, "BottomBorder")
    __write_prop(xml, buf, 0x02, 0x02, 0x16, "Цвет_узора")
    __write_prop(xml, buf, 0x01, 0x02, 0x17, "BorderColor")
    __write_prop(xml, buf, 0x01, 0x40, 0x18, "TextColor")
    __write_prop(xml, buf, 0x01, 0x80, 0x19, "BackgroundColor")
    __write_prop(xml, buf, 0x02, 0x04, 0x1a, "Control")
    __write_prop(xml, buf, 0x02, 0x08, 0x1b, "Тип")
    __write_prop(xml, buf, 0x02, 0x10, 0x1c, "Protection")
    # row height/page header
    flag, = unpack("=B",buf[0x01])
    if flag & 0x04:
        share1_val, = unpack("=H",buf[0x04:0x06])
        __write_element(xml,share1,str(share1_val))
    # col width/page header
    #flag, = unpack("=B",buf[0x01])
    if flag & 0x08:
        share2_val, = unpack("=H",buf[0x06:0x08])
        __write_element(xml,share2,str(share2_val))
    #xml.characters(str([buf]))
    # text, detail
    flag, = unpack("=B",buf[3])
    if flag & 0x80:
        size3, = unpack("=B", fi.read(1))
        __write_element(xml,"Text",fi.read(size3))
    if flag & 0x40:
        size4, = unpack("=B", fi.read(1))
        __write_element(xml,"Details",fi.read(size4))
    xml.endElement("cell")

def __objs(fi, xml):
    size1, = unpack("=H", fi.read(2))
    if size1:
        xml.startElement("objs",{"size":str(size1)})
        for i in xrange(size1):
            xml.startElement("obj",{"id":str(i+1)})
            __cell(fi, xml, 0)
            buf = fi.read(40)
            #print [buf]
            tmp = unpack("=10i", buf)
            __write_element(xml,"ТипОбъекта",str(tmp[0]))
            __write_element(xml,"КолНач",str(tmp[1]))
            __write_element(xml,"СтрНач",str(tmp[2]))
            __write_element(xml,"XНач",str(tmp[3]))
            __write_element(xml,"YНач",str(tmp[4]))
            __write_element(xml,"КолКон",str(tmp[5]))
            __write_element(xml,"СтрКон",str(tmp[6]))
            __write_element(xml,"XКон",str(tmp[7]))
            __write_element(xml,"YКон",str(tmp[8]))
            __write_element(xml,"Уровень",str(tmp[9]))
            if tmp[0]==5:
                # рисунок
                tmp2 = unpack("=2i", fi.read(8))
                buf = fi.read(tmp2[1])
                __write_element(xml,"Рисунок","ОЙ, надо поискать ...:)")
            if tmp[0]==4:
                # OLE
                tmp2 = unpack("=iH", fi.read(6))
                #print tmp2
                # type
                buf = fi.read(tmp2[1])
                #print [buf]
                tmp3 = unpack("=3iH2i", fi.read(22))
                #print tmp3
                # OLE
                buf = fi.read(tmp3[5])
                __write_element(xml,"ОЛЕ","ОЙ, где то потерялся ...:)")
            #__col(fi, xml)
            xml.endElement("obj")
        xml.endElement("objs")
    
def __merge(fi, xml):
    size1, = unpack("=H", fi.read(2))
    if size1:
        xml.startElement("merge",{"size":str(size1)})
        for i in xrange(size1):
            buf = fi.read(16)
            begin_c, begin_r, end_c, end_r = unpack("=4i", buf)
            # xml
            xml.startElement("merge_el",{"id":str(i+1)})
            __write_element(xml,"begin_col",str(begin_c))
            __write_element(xml,"begin_row",str(begin_r))
            __write_element(xml,"end_col",str(end_c))
            __write_element(xml,"end_row",str(end_r))
            xml.endElement("merge_el")
        xml.endElement("merge")

def __sects(fi, xml, main, child):
    size1, = unpack("=H", fi.read(2))
    if size1:
        xml.startElement(main,{"size":str(size1)})
        for i in xrange(size1):
            buf = fi.read(13)
            begin, end, parent, _unk, size_name = unpack("=iiHHB", buf)
            name = fi.read(size_name)
            # xml
            xml.startElement(child,{"id":str(i+1)})
            __write_element(xml,"begin",str(begin))
            __write_element(xml,"end",str(end))
            __write_element(xml,"parent",str(parent))
            __write_element(xml,"name",name)
            xml.endElement(child)
        xml.endElement(main)

def __pagefeeds(fi, xml, main, child):
    size1, = unpack("=H", fi.read(2))
    if size1:
        buf = fi.read(size1*4)
        numbs = unpack("="+"i"*size1, buf)
        xml.startElement(main,{"size":str(size1)})
        for i in xrange(size1):
            # xml
            xml.startElement(child,{"id":str(i+1)})
            xml.characters(str(numbs[i]))
            xml.endElement(child)
        xml.endElement(main)

def __names(fi, xml):
    size1, = unpack("=i", fi.read(4))
    if size1:
        xml.startElement("names",{"size":str(size1)})
        for i in xrange(size1):
            xml.startElement("name",{"id":str(i+1)})
            size_nm, = unpack("=B", fi.read(1))
            __write_element(xml,"NameText",fi.read(size_nm))
            buf = fi.read(28)
            _unk1, _unk2, _unk3, begin_col, begin_row, end_col, end_row = unpack("=7i", buf)
            __write_element(xml,"_unk1",str(_unk1))
            __write_element(xml,"_unk2",str(_unk2))
            __write_element(xml,"_unk3",str(_unk3))
            __write_element(xml,"BeginCol",str(begin_col))
            __write_element(xml,"BeginRow",str(begin_row))
            __write_element(xml,"EndCol",str(end_col))
            __write_element(xml,"EndRow",str(end_row))
            xml.endElement("name")
        xml.endElement("names")
        
mxl2xml("Page.mxl")

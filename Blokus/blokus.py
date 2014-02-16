import pygame,sys,math
from pygame.locals import *

WINDOWWIDTH=1280
WINDOWHEIGHT=800
BOXSIZE=25
GAPSIZE=1 # is the gap of the lines
BOARDWIDTH=14 # number of columns of boxes
BOARDHEIGHT=14 # number of rows of boxes
ROTATIONBOX=120 #size of the box where rotation happens

XMARGIN = int ((WINDOWWIDTH - (BOARDWIDTH * (BOXSIZE + GAPSIZE))) / 2)
YMARGIN = int((WINDOWHEIGHT - (BOARDHEIGHT * (BOXSIZE + GAPSIZE))) / 2)

#            R    G    B
GRAY     = (100, 100, 100)
DARKGRAY = (200, 200, 200)
NAVYBLUE = ( 60,  60, 100)
WHITE    = (255, 255, 255)
RED      = (255,   0,   0)
GREEN    = (  0, 255,   0)
BLUE     = (  0,   0, 255)
YELLOW   = (255, 255,   0)
ORANGE   = (255, 128,   0)
PURPLE   = (255,   0, 255)
CYAN     = (  0, 255, 255)

BGCOLOR = NAVYBLUE
BOXCOLOR = GRAY
LINESCOLOR= DARKGRAY

def main():
    global DISPLAYSURF, state, player, list_boxes, list_obj, list_obj1, list_obj2, list_boxes1, list_boxes2, list_buttons, outcome
    pygame.init()
    DISPLAYSURF = pygame.display.set_mode((WINDOWWIDTH, WINDOWHEIGHT))
    DISPLAYSURF.fill(BGCOLOR)

    mousex, mousey = 0, 0
    list_obj=[]
    list_obj1=[]
    list_obj2=[]
    list_boxes=[]
    list_boxes1=[]
    list_boxes2=[]
    list_buttons=[]
    state = "ready"
    player = 1
    outcome = "First shape of each color should touch the colored corners "

    list_obj = list_obj1
    list_boxes = list_boxes1
    
    create_rotation_box()
    create_objects()# create an object of each shape
    create_side_boxes()#create the boxes at the side
    shape_pos()#give at the shapes the right first positions
    clickedBox = None #clicked box at the side bar
    clicked = False #shows if there is a shape clicked
    clickedShape = None #shape that is clicked
  
    while True:
        DISPLAYSURF.fill(BGCOLOR) # drawing the window
        #draw rect with the color of the player which is his turn to play
        if player == 1:
            pygame.draw.rect(DISPLAYSURF, ORANGE, Rect(WINDOWWIDTH/2-50, 10, 100, 100))
        else:
            pygame.draw.rect(DISPLAYSURF, CYAN, Rect(WINDOWWIDTH/2-50, 10, 100, 100))

        drawBoard()
        sideBoard()
        create_rotation_box()
        place_shapes()
        arrows()
        draw_circles()
        draw_texts()

        for event in pygame.event.get():
            if event.type == MOUSEBUTTONUP:
                mousex, mousey = event.pos
                clicked = True
            elif event.type == MOUSEMOTION:
                mousex, mousey = event.pos

        if clicked == True:          
            if getBoxAtPixel(mousex,mousey) != None:
                #if this shape is not placed yet on the board
                if list_obj[getBoxAtPixel(mousex,mousey)].state == 1:
                    clickedBox = getBoxAtPixel(mousex,mousey)
                    state = "rotationbox"
                if clickedShape != None and clickedShape.state == 1:
                    move_to_starting_position(clickedShape)
            clicked = False
            #if restart button is clicked
            if clicked_button(mousex, mousey) == 4:
                main()
            if clickedBox != None:
                clickedShape = list_obj[clickedBox]
                if state == "position":
                    if align_click(mousex, mousey, clickedShape) == 1:
                        if rules(mousex, mousey, clickedShape) == 1:
                            clickedShape.state = 0 # it is placed
                            state = "ready"
                            outcome = ""
                            change_player(player)
                elif state == "rotationbox":
                    #position of shape is in the rotation square
                    translate_to_rotation(clickedShape)
                    align(clickedShape, list_buttons[0])
                    state = "rotate"
                elif state == "rotate":
                        rotation(clickedShape, clicked_button(mousex, mousey))
                        align(clickedShape, list_buttons[0])
                    
        elif state == "position":
            #change it's position according to mouse position
            diffx = mousex-clickedShape.listRect[0].left
            diffy = mousey-clickedShape.listRect[0].top
            for i in range(len(clickedShape.listRect)):
                clickedShape.listRect[i].top+=diffy
                clickedShape.listRect[i].left+=diffx   

        pygame.display.update()


def draw_texts():
    global DISPLAYSURF, list_buttons
    #draw text with the proper message
    BASICFONT = pygame.font.Font('freesansbold.ttf', 16)
    infoSurf = BASICFONT.render(outcome, 1, WHITE)
    infoRect = infoSurf.get_rect()
    infoRect.topleft = (WINDOWWIDTH/2 - infoRect.width/2, YMARGIN - 100)
    DISPLAYSURF.blit(infoSurf, infoRect)
    #draw text with score
    infoSurf = BASICFONT.render(str(score()[0]), 1, ORANGE)
    infoRect = infoSurf.get_rect()
    infoRect.topleft = (XMARGIN, 50)
    DISPLAYSURF.blit(infoSurf, infoRect)
    infoSurf = BASICFONT.render(str(score()[1]), 1, CYAN)
    infoRect = infoSurf.get_rect()
    infoRect.topleft = (WINDOWWIDTH-XMARGIN, 50)
    DISPLAYSURF.blit(infoSurf, infoRect)
    #draw the TEXT of restart button
    infoSurf = BASICFONT.render("Restart", 1, GRAY)
    infoRect = infoSurf.get_rect()
    infoRect.topleft = (list_buttons[4].left+BOXSIZE/2, list_buttons[4].top+BOXSIZE/2)
    DISPLAYSURF.blit(infoSurf, infoRect)
    
#calculate score for each color (remaining boxes of shapes)
def score():
    global list_obj1, list_obj2
    boxes1 = 0
    boxes2 = 0
    for i in range(len(list_obj1)):
        if list_obj1[i].state == 1:
            boxes1 += len(list_obj1[i].listRect)
    for i in range(len(list_obj2)):
        if list_obj2[i].state == 1:
            boxes2 += len(list_obj2[i].listRect)
    return (boxes1, boxes2)

#draw circles at the position that the player should place the first shapes
def draw_circles():
    global DISPLAYSURF
    pygame.draw.circle(DISPLAYSURF,ORANGE,(int(WINDOWWIDTH/2-13*BOXSIZE/2),int(YMARGIN+13.5*BOXSIZE)),int(BOXSIZE/2), 3)
    pygame.draw.circle(DISPLAYSURF,CYAN,(int(XMARGIN+14*BOXSIZE),int(YMARGIN+BOXSIZE/2)),int(BOXSIZE/2), 3)

def change_player(pl):
    global player, list_obj, list_obj1, list_obj2, list_boxes, list_boxes1, list_boxes2
    
    if player == 1:
        player = 2
        list_obj = list_obj2
        list_boxes = list_boxes2
    else:
        player = 1
        list_obj = list_obj1
        list_boxes = list_boxes1

##all parts of shape must be inside the game board
def rule1(board, shape):
    global outcome
    for i in range(len(shape.listRect)):
        if board.colliderect(Rect(shape.listRect[i].left, shape.listRect[i].top, BOXSIZE, BOXSIZE)) == False:
                outcome = "all parts of shape must be inside the game board"
                return 0
    return 1

#recognize first click
def rule2():
    global list_obj
    for i in range(len(list_obj)):
        if list_obj[i].state == 1:
            continue
        else:
            return 0 # it is not the first move
    return 1 #it is the first move

#first shape should have certain position
def rule3(bx, by, shape):
    global player, outcome
    for i in range(len(shape.listRect)):
        if player == 1:
        #if there is a box at the left bottom corner then return true. it is correct!
            if shape.listRect[i].left == bx and shape.listRect[i].top == by+13*BOXSIZE:
                return 1
        else:
            if shape.listRect[i].left == bx+13*BOXSIZE and shape.listRect[i].top == by:
                return 1
    outcome = "First shape of each color should touch the colored corners"
    return 0

#boxes of any color should not collide
def rule4(shape):
    global list_obj1, list_obj2, outcome
    for i in range(len(shape.listRect)):
        tempbox = Rect(shape.listRect[i].left, shape.listRect[i].top, BOXSIZE, BOXSIZE)
        #test if it is collide with any other box in any shape
        for j in range(len(list_obj)):
            if list_obj1[j].state == 0: # check only shapes that are already placed in orange color
                for k in range(len(list_obj1[j].listRect)):
                #if 2 boxes collide
                    if tempbox.colliderect(Rect(list_obj1[j].listRect[k].left, list_obj1[j].listRect[k].top, BOXSIZE, BOXSIZE)) == True:
                        outcome = "boxes of any color should not collide"
                        return 0
            if list_obj2[j].state == 0: #check shapes in cyan color
                for k in range(len(list_obj2[j].listRect)):
                #if 2 boxes collide
                    if tempbox.colliderect(Rect(list_obj2[j].listRect[k].left, list_obj2[j].listRect[k].top, BOXSIZE, BOXSIZE)) == True:
                        outcome = "boxes of any color should not collide"
                        return 0
    return 1

#shapes with same color must connect at least in one corner
def rule5(shape):
    global list_obj, outcome
    for i in range(len(shape.listRect)):
        for j in range(len(list_obj)):
            if list_obj[j].state == 0:
                for k in range(len(list_obj[j].listRect)):
                    if math.fabs(shape.listRect[i].left - list_obj[j].listRect[k].left) == BOXSIZE and math.fabs(shape.listRect[i].top - list_obj[j].listRect[k].top) == BOXSIZE:
                        return 1
    outcome = "shapes with same color must connect at least in one corner"
    return 0

#shapes with the same color should not touch each other
def rule6(shape):
    global list_obj, outcome
    for i in range(len(shape.listRect)):
        for j in range(len(list_obj)):
            if list_obj[j].state == 0:
                for k in range(len(list_obj[j].listRect)):
                    if shape.listRect[i].left == list_obj[j].listRect[k].left and math.fabs(shape.listRect[i].top - list_obj[j].listRect[k].top) == BOXSIZE:
                        outcome = "shapes with the same color should not be side by side"
                        return 0
                    elif shape.listRect[i].top == list_obj[j].listRect[k].top and math.fabs(shape.listRect[i].left - list_obj[j].listRect[k].left) == BOXSIZE:
                        outcome = "shapes with the same color should not be side by side"
                        return 0
    return 1

    
#define the rules of game
def rules(mx, my, shape):
    global player, list_obj, list_obj1, list_obj2, outcome
    bx = WINDOWWIDTH/2 - 14*BOXSIZE/2 #board's x pos
    by = YMARGIN
    width = 14*BOXSIZE
    board = pygame.Rect(bx,by,width,width)
    if rule1(board,shape): #be sure that it is on the game board  board.collidepoint(mx,my)
        if rule2() == 1:
            return rule3(bx, by, shape) 
        else:
            return rule4(shape) and rule5(shape) and rule6(shape)
    else:
        outcome = "shapes should be on the game board"
                   
#align the mouse click in the right box of the board
def  align_click(mx, my, shape):
    bx = WINDOWWIDTH/2 - 14*BOXSIZE/2 #board's x pos
    by = YMARGIN
    width = 14*BOXSIZE
    pos = [-1, -1]
    if pygame.Rect(bx,by,width,width).collidepoint(mx,my):
        #that's the right box coord on the board that is clicked
        pos = [bx + int((mx - bx)/BOXSIZE)*BOXSIZE,by + int((my - by)/BOXSIZE)*BOXSIZE]
        diffx = pos[0] - shape.listRect[0].left
        diffy = pos[1] - shape.listRect[0].top
        #move the shape at these coord
        for i in range(len(shape.listRect)):
            shape.listRect[i].left += diffx
            shape.listRect[i].top += diffy
        return 1
    else:
        return 0
        
    
#check which rotation button is clicked
def clicked_button(mx, my):
    global list_buttons
    for i in range(len(list_buttons)):
        if i == 0:
            boxRect = pygame.Rect(list_buttons[i].left,list_buttons[i].top,ROTATIONBOX,ROTATIONBOX)
        elif i == 4:
            boxRect = pygame.Rect(list_buttons[i].left,list_buttons[i].top,list_buttons[i].width,list_buttons[i].height)
        else:
            boxRect = pygame.Rect(list_buttons[i].left,list_buttons[i].top,ROTATIONBOX/3,ROTATIONBOX/3)
        if boxRect.collidepoint(mx,my):
            return i

#align shapes in the boxes that they should be
def align(shape, box):
    #find the min values of the most "left" and "up" box
    minx = shape.listRect[0].left
    miny = shape.listRect[0].top
    x , y = 0, 0
    for i in range(len(shape.listRect)):
        if shape.listRect[i].left < minx:
            minx = shape.listRect[i].left
        if shape.listRect[i].top < miny:
            miny = shape.listRect[i].top
    #move all boxes
    if box.left > minx:
        x = box.left - minx #0-minx
    if box.top > miny:
        y = box.top - miny
    for i in range(len(shape.listRect)):
        shape.listRect[i].left += x
        shape.listRect[i].top += y

def move_to_starting_position(shape):
    global list_obj, list_boxes
    box = -1
    #which object is it
    for i in range(len(list_obj)):
        if list_obj[i] == shape:
            box = i
    #move to box at the side bar at position list_boxes[box]
    diffx = shape.listRect[0].left - list_boxes[box].left
    diffy = shape.listRect[0].top - list_boxes[box].top
    for i in range(len(shape.listRect)):
        shape.listRect[i].left -= diffx
        shape.listRect[i].top -= diffy

    align(shape, list_boxes[box])
    

def translate_to_rotation(shape):
    global list_buttons
    #list_buttons[0] is the rotation square
    diffx = list_buttons[0].left-shape.listRect[0].left
    diffy = list_buttons[0].top-shape.listRect[0].top
    for i in range(len(shape.listRect)):
        shape.listRect[i].left += diffx
        shape.listRect[i].top += diffy

    
#translation to origin, rotation, translation again
def rotation(shape, x):
    #move to origin
    diffx = shape.listRect[0].left
    diffy = shape.listRect[0].top
    translation(shape,-diffx, -diffy)#list_obj[11]
    #rotate in place
    rotate_origin(shape,x)
    #move to the desired location
    translation(shape,diffx,diffy)#list_obj[11]

#translation
def translation(shape, x, y):
    for i in range(len(shape.listRect)):
        shape.listRect[i].left += x
        shape.listRect[i].top += y

#rotate an object which must be at the origin
def rotate_origin(shape,x):
    global state
    if x == 0:
        state = "position"
    for i in range(len(shape.listRect)):
        previous_left = shape.listRect[i].left
        previous_top = shape.listRect[i].top
        if x == 1:
            shape.listRect[i].top = previous_left*int(math.sin(math.pi))+previous_top*int(math.cos(math.pi))
        elif x == 2:
            shape.listRect[i].left = previous_left*int(math.cos(math.pi))-previous_top*int(math.sin(math.pi))
        elif x == 3:
            shape.listRect[i].left = previous_left*int(math.cos(math.pi/2))-previous_top*int(math.sin(math.pi/2))
            shape.listRect[i].top = previous_left*int(math.sin(math.pi/2))+previous_top*int(math.cos(math.pi/2))
            
#draw the main board
def drawBoard():
    x = WINDOWWIDTH/2 - 14*BOXSIZE/2
    pygame.draw.rect(DISPLAYSURF, BOXCOLOR, (x,YMARGIN, 14*BOXSIZE, 14*BOXSIZE))
    for i in range(15):
        pygame.draw.line(DISPLAYSURF,DARKGRAY,(x+i*BOXSIZE,YMARGIN),(x+i*BOXSIZE,YMARGIN+14*BOXSIZE))
    for j in range(15):
        pygame.draw.line(DISPLAYSURF,DARKGRAY,(x,YMARGIN+j*BOXSIZE),(x+14*BOXSIZE,YMARGIN+j*BOXSIZE))

#draw the lines at the side to put there the shape objects
def sideBoard():
    num=WINDOWHEIGHT/10 #num size of boxes at the side
    for i in range(3):
        pygame.draw.line(DISPLAYSURF,DARKGRAY,(i*BOXSIZE*6,0),(i*BOXSIZE*6,WINDOWHEIGHT))
        pygame.draw.line(DISPLAYSURF,DARKGRAY,(WINDOWWIDTH-i*BOXSIZE*6,0),(WINDOWWIDTH-i*BOXSIZE*6,WINDOWHEIGHT))
    for j in range(11):
        pygame.draw.line(DISPLAYSURF,DARKGRAY,(0,j*num),(BOXSIZE*12,j*num))
        pygame.draw.line(DISPLAYSURF,DARKGRAY,(WINDOWWIDTH,j*num),(WINDOWWIDTH-BOXSIZE*12,j*num))

#draw arrowa
def arrows():
    #first arrow
    x=list_buttons[1].left + ROTATIONBOX/6
    x2=list_buttons[1].top + 2
    y=list_buttons[1].top + ROTATIONBOX/6
    pygame.draw.line(DISPLAYSURF,GRAY,(x,x2),(x,y),3)
    pygame.draw.line(DISPLAYSURF,GRAY,(x,y),(x-5,y-5),3)
    pygame.draw.line(DISPLAYSURF,GRAY,(x,y),(x+5,y-5),3)
    #second arrow
    x=list_buttons[2].left + 2
    x2=list_buttons[2].top + ROTATIONBOX/6
    pygame.draw.line(DISPLAYSURF,GRAY,(x,x2),(x+ROTATIONBOX/6,x2),3)
    pygame.draw.line(DISPLAYSURF,GRAY,(x+ROTATIONBOX/6,x2),(x+ROTATIONBOX/6-5,x2-5),3)
    pygame.draw.line(DISPLAYSURF,GRAY,(x+ROTATIONBOX/6,x2),(x+ROTATIONBOX/6-5,x2+5),3)
    #third arrow - circle
    x=list_buttons[3].left
    y=list_buttons[3].top
    pygame.draw.circle(DISPLAYSURF,GRAY,(int(x+ROTATIONBOX/6),int(y+ROTATIONBOX/6)),int(ROTATIONBOX/6), 3)

#θα πρεπει να το σπασω σε 2 συναρτησεις, μια θα τα δημιουργει κι αλλη θα τα τυπωνει   
def create_rotation_box():
    global list_buttons
    pos_x = WINDOWWIDTH/2-ROTATIONBOX/2
    pos_y = WINDOWHEIGHT-YMARGIN
    #central box (for rendering)
    b=box(pos_x,pos_y,GRAY)
    list_buttons.append(pygame.Rect(b.left,b.top,ROTATIONBOX,ROTATIONBOX))
    pygame.draw.rect(DISPLAYSURF, b.color, list_buttons[0])
    #left up button
    b1=box(pos_x - ROTATIONBOX/3 - GAPSIZE,pos_y,DARKGRAY)
    list_buttons.append(pygame.Rect(b1.left,b1.top,ROTATIONBOX/3,ROTATIONBOX/3))
    pygame.draw.rect(DISPLAYSURF, b1.color, list_buttons[1])
    #left middle button
    b2=box(pos_x - ROTATIONBOX/3 - GAPSIZE,pos_y + ROTATIONBOX/3 + GAPSIZE,DARKGRAY)
    list_buttons.append(pygame.Rect(b2.left,b2.top,ROTATIONBOX/3,ROTATIONBOX/3))
    pygame.draw.rect(DISPLAYSURF, b2.color, list_buttons[2])
    #left down button
    b3=box(pos_x - ROTATIONBOX/3 - GAPSIZE,pos_y + 2*ROTATIONBOX/3 + 2*GAPSIZE,DARKGRAY)
    list_buttons.append(pygame.Rect(b3.left,b3.top,ROTATIONBOX/3,ROTATIONBOX/3))
    pygame.draw.rect(DISPLAYSURF, b3.color, list_buttons[3])
    #restart button
    b4 = box(WINDOWWIDTH/2-1.5*BOXSIZE,WINDOWHEIGHT-BOXSIZE*3,DARKGRAY)
    list_buttons.append(pygame.Rect(b4.left,b4.top,3*BOXSIZE,BOXSIZE))
    pygame.draw.rect(DISPLAYSURF,b4.color,list_buttons[4])
    
    
#place a shape object in each side box
def shape_pos():
    global list_obj1, list_obj2, list_boxes1, list_boxes2
    for i in range(len(list_obj)):
        for j in range(len(list_obj[i].listRect)):
            list_obj1[i].listRect[j].top+=list_boxes1[i].top
            list_obj1[i].listRect[j].left+=list_boxes1[i].left
            list_obj2[i].listRect[j].top+=list_boxes2[i].top
            list_obj2[i].listRect[j].left+=list_boxes2[i].left
            
#draw shapes at the side            
def place_shapes():
    global DISPLAYSURF, list_obj1, list_obj2
    for i in range(len(list_obj1)):
         for j in range(len(list_obj1[i].listRect)):
             pygame.draw.rect(DISPLAYSURF, ORANGE, list_obj1[i].listRect[j])
    for i in range(len(list_obj2)):
         for j in range(len(list_obj2[i].listRect)):
             pygame.draw.rect(DISPLAYSURF, CYAN, list_obj2[i].listRect[j])


#create one object of each class and save the objects in a list
def create_objects():
    global list_obj1, list_obj2
    list_obj1.append(shape_line5())
    list_obj1.append(shape_line4())
    list_obj1.append(shape_line3())
    list_obj1.append(shape_line2())
    list_obj1.append(shape_P())
    list_obj1.append(shape_cross())
    list_obj1.append(shape_g1())
    list_obj1.append(shape_z())
    list_obj1.append(shape_g())
    list_obj1.append(shape_gl())
    list_obj1.append(shape_gg())
    list_obj1.append(shape_T())
    list_obj1.append(shape_box())
    list_obj1.append(shape_square())
    list_obj1.append(shape_corner())
    list_obj1.append(shape_greekL())
    list_obj1.append(shape_greekP())
    list_obj1.append(shape_longZ())
    list_obj1.append(shape_z1())
    list_obj1.append(shape_stairs())

    list_obj2.append(shape_line5())
    list_obj2.append(shape_line4())
    list_obj2.append(shape_line3())
    list_obj2.append(shape_line2())
    list_obj2.append(shape_P())
    list_obj2.append(shape_cross())
    list_obj2.append(shape_g1())
    list_obj2.append(shape_z())
    list_obj2.append(shape_g())
    list_obj2.append(shape_gl())
    list_obj2.append(shape_gg())
    list_obj2.append(shape_T())
    list_obj2.append(shape_box())
    list_obj2.append(shape_square())
    list_obj2.append(shape_corner())
    list_obj2.append(shape_greekL())
    list_obj2.append(shape_greekP())
    list_obj2.append(shape_longZ())
    list_obj2.append(shape_z1())
    list_obj2.append(shape_stairs())

#the clicked box at the side board
def getBoxAtPixel(x,y):
    global list_boxes
    for i in range(len(list_boxes)):
        boxRect = pygame.Rect(list_boxes[i].left,list_boxes[i].top,BOXSIZE*6,WINDOWHEIGHT/10)
        if boxRect.collidepoint(x,y):
            return i
    return None

#create an object for each box at the side board
#in these boxes we will put the shape objects
def create_side_boxes():
    num=WINDOWHEIGHT/10 #num size of boxes at the side
    global list_boxes1, list_boxes2
    for i in range(10):
        list_boxes1.append(box(0,num*i))
        list_boxes1.append(box(BOXSIZE*6,num*i))
        list_boxes2.append(box(WINDOWWIDTH-BOXSIZE*12,num*i))
        list_boxes2.append(box(WINDOWWIDTH-BOXSIZE*6,num*i))
  
#define a box
class box:
    def __init__(self,l,t,c=RED):
        self.top=t
        self.left=l
        self.color=c

    def get_left(self):
        return self.left

    def get_top(self):
        return self.top

    def set_left(self,l):
        self.left=l

    def set_top(self,t):
        self.top=t
        
#define a shape like a line with 5 boxes     
class shape_line5(box):
    def __init__(self):
        self.listRect=[]
        self.state = 1
        for i in range(5):
            b=box(i*BOXSIZE,0)
            self.listRect.append(pygame.Rect(b.left,b.top,BOXSIZE,BOXSIZE))

#define a shape like a line with 4 boxes     
class shape_line4(box):
    def __init__(self):
        self.listRect=[]
        self.state = 1
        for i in range(4):
            b=box(i*BOXSIZE,0)
            self.listRect.append(pygame.Rect(b.left,b.top,BOXSIZE,BOXSIZE))

#define a shape like a line with 3 boxes     
class shape_line3(box):
    def __init__(self):
        self.listRect=[]
        self.state = 1
        for i in range(3):
            b=box(i*BOXSIZE,0)
            self.listRect.append(pygame.Rect(b.left,b.top,BOXSIZE,BOXSIZE))

#define a shape like a line with 2 boxes     
class shape_line2(box):
    def __init__(self):
        self.listRect=[]
        self.state = 1
        for i in range(2):
            b=box(i*BOXSIZE,0)
            self.listRect.append(pygame.Rect(b.left,b.top,BOXSIZE,BOXSIZE))

#define a shape like a box     
class shape_box(box):
    def __init__(self):
        self.listRect=[]
        self.state = 1
        b=box(0,0)
        self.listRect.append(pygame.Rect(b.left,b.top,BOXSIZE,BOXSIZE))

#define a shape like +
class shape_cross(box):
    def __init__(self):
        self.listRect=[]
        self.state = 1
        b=box(BOXSIZE,0)
        self.listRect.append(pygame.Rect(b.left,b.top,BOXSIZE,BOXSIZE))
        b=box(0,BOXSIZE)
        self.listRect.append(pygame.Rect(b.left,b.top,BOXSIZE,BOXSIZE))
        b=box(BOXSIZE,BOXSIZE)
        self.listRect.append(pygame.Rect(b.left,b.top,BOXSIZE,BOXSIZE))
        b=box(2*BOXSIZE,BOXSIZE)
        self.listRect.append(pygame.Rect(b.left,b.top,BOXSIZE,BOXSIZE))
        b=box(BOXSIZE,2*BOXSIZE)
        self.listRect.append(pygame.Rect(b.left,b.top,BOXSIZE,BOXSIZE))

#define a shape like greek g with an extra box on top (Γ)
class shape_g1(box):
    def __init__(self):
        self.listRect=[]
        self.state = 1
        for i in range(4):
            if i==3:
                b=box(BOXSIZE,BOXSIZE)
            else:
                b=box(0,i*BOXSIZE)
            self.listRect.append(pygame.Rect(b.left,b.top,BOXSIZE,BOXSIZE))

#define a shape like greek g (Γ)
class shape_g(box):
    def __init__(self):
        self.listRect=[]
        self.state = 1
        for i in range(4):
            if i<=2:
                b=box(0,i*BOXSIZE)
            else:
                b=box(BOXSIZE,0)
            self.listRect.append(pygame.Rect(b.left,b.top,BOXSIZE,BOXSIZE))

class shape_z(box):
    def __init__(self):
        self.listRect=[]
        self.state = 1
        for i in range(4):
            if i == 0 or i == 1:
                b=box(i*BOXSIZE,0)
            else:
                b=box((i-1)*BOXSIZE,BOXSIZE)
            self.listRect.append(pygame.Rect(b.left,b.top,BOXSIZE,BOXSIZE))

#define shape like greek g and L together
class shape_gl(box):
    def __init__(self):
        self.listRect=[]
        self.state = 1
        for i in range(5):
            if i==0 or i==4:
                b=box(BOXSIZE+BOXSIZE*i/4,BOXSIZE*i/2)
            else:
                b=box((i-1)*BOXSIZE,BOXSIZE)
            self.listRect.append(pygame.Rect(b.left,b.top,BOXSIZE,BOXSIZE))

#define shape like g but longer
class shape_gg(box):
    def __init__(self):
        self.listRect=[]
        self.state = 1
        for i in range(5):
            if i==4:
                b=box(BOXSIZE,0)
            else:
                b=box(i*BOXSIZE,BOXSIZE)
            self.listRect.append(pygame.Rect(b.left,b.top,BOXSIZE,BOXSIZE))

#define shape like T
class shape_T(box):
    def __init__(self):
        self.listRect=[]
        self.state = 1
        for i in range(5):
            if i<=2:
                b=box(i*BOXSIZE,0)
            else:
                b=box(BOXSIZE,math.floor(i/2)*BOXSIZE)
            self.listRect.append(pygame.Rect(b.left,b.top,BOXSIZE,BOXSIZE))

#define shape like square + one more box (P)
class shape_P(box):
    def __init__(self):
        self.listRect=[]
        self.state = 1
        for i in range(5):
            if i<=2:
                b=box(0,i*BOXSIZE)
            else:
                b=box(BOXSIZE,(i-3)*BOXSIZE)
            self.listRect.append(pygame.Rect(b.left,b.top,BOXSIZE,BOXSIZE))

#define shape like square 
class shape_square(box):
    def __init__(self):
        self.listRect=[]
        self.state = 1
        for i in range(4):
            if i<=1:
                b=box(0,i*BOXSIZE)
            else:
                b=box(BOXSIZE,(i-2)*BOXSIZE)
            self.listRect.append(pygame.Rect(b.left,b.top,BOXSIZE,BOXSIZE))

#define a shape like a corner  
class shape_corner(box):
    def __init__(self):
        self.listRect=[]
        self.state = 1
        for i in range(3):
            if i<=1:
                b=box(i*BOXSIZE,0)
            else:
                b=box(BOXSIZE,BOXSIZE)
            self.listRect.append(pygame.Rect(b.left,b.top,BOXSIZE,BOXSIZE))

#define a shape like greek l (Λ) 
class shape_greekL(box):
    def __init__(self):
        self.listRect=[]
        self.state = 1
        for i in range(5):
            if i<=2:
                b=box(i*BOXSIZE,0)
            else:
                b=box(2*BOXSIZE,math.floor(i/2)*BOXSIZE)
            self.listRect.append(pygame.Rect(b.left,b.top,BOXSIZE,BOXSIZE))

#define a shape like greek p(Π) 
class shape_greekP(box):
    def __init__(self):
        self.listRect=[]
        self.state = 1
        for i in range(5):
            if i<=2:
                b=box(0,i*BOXSIZE)
            elif i==3:
                b=box(BOXSIZE,0)
            elif i==4:
                b=box(BOXSIZE,2*BOXSIZE)
            self.listRect.append(pygame.Rect(b.left,b.top,BOXSIZE,BOXSIZE))

#define a shape like long Z
class shape_longZ(box):
    def __init__(self):
        self.listRect=[]
        self.state = 1
        for i in range(5):
            if i<=2:
                b=box(BOXSIZE,i*BOXSIZE)
            elif i==3:
                b=box(0,0)
            elif i==4:
                b=box(i/2*BOXSIZE,i/2*BOXSIZE)
            self.listRect.append(pygame.Rect(b.left,b.top,BOXSIZE,BOXSIZE))

#define shape like z with one more box
class shape_z1(box):
    def __init__(self):
        self.listRect=[]
        self.state = 1
        for i in range(5):
            if i <=1:
                b=box(i*BOXSIZE,0)
            else:
                b=box((i-1)*BOXSIZE,BOXSIZE)
            self.listRect.append(pygame.Rect(b.left,b.top,BOXSIZE,BOXSIZE))

#define shape like stairs
class shape_stairs(box):
    def __init__(self):
        self.listRect=[]
        self.state = 1
        for i in range(5):
            if i <=1:
                b=box(0,(i+1)*BOXSIZE)
            elif i<=3:
                b=box(BOXSIZE,(i-2)*BOXSIZE)
            else:
                b=box(2*BOXSIZE,0)
            self.listRect.append(pygame.Rect(b.left,b.top,BOXSIZE,BOXSIZE))

if __name__ == '__main__':
    main()

# -*- coding: utf-8 -*-
"""
Created on Fri Aug 16 09:17:06 2019

@authors: Jørgen Winther Søbstad, Bogdan Valentin Raicu
"""
import random as ra
import numpy as np
import pygame, sys
import time
from Fishgame_visuals import *
from pygame.locals import *


clock = pygame.time.Clock()
pygame.init()
pygame.font.init()


pygame.display.set_caption("FISHERMAN") #name on top of window
screen_res = (800,400) #Screen resolution
screen = pygame.display.set_mode((screen_res)) #game window , pygame.FULLSCREEN
myfont = pygame.font.SysFont("monospace", 16)

#Animation info#
cloud_1_pos = [-550, 0]
sea_pos = [0, 370]
sea_2_pos = [0, 360]
sea_speed = 0
idleAnimationCount = 0

#PLAYER POS INFO#
player_pos = [150,275]
moving_left = False
moving_right = False
dupp_pos = player_pos

#FISHING GEAR
stang_1 = 11
snelle_1 = 25
stang_2 = 20    
snelle_2 = 100
stang = stang_2
snelle = snelle_2

########START GAME##########
start_font = pygame.font.SysFont('monospace', 32)
start_text = start_font.render("Start Game", True, (0, 0, 0))
start_text_rect = start_text.get_rect()
start_text_rect.center = (800//2, 400//2)


#########FISH DICT##############
fish_caught_lib = {'trash':0,'rare':0,'legendary':0}

######MAIN MENU###########
def start_screen():
    main_menu = True    
        
    height = 100
    width = 200
    
    x = 300
    y = 150
    r = 0
    g = 255
       
    while main_menu == True:        
        
        screen.fill((0,0,0))
        pygame.draw.rect(screen, (r,g,0), pygame.Rect(x, y, width, height))
        screen.blit(start_text, start_text_rect)
        pygame.display.update()
        
        for event in pygame.event.get():
             
            if event.type == QUIT:  #Exit Game function           
                pygame.quit()               
                sys.exit()
                
            if event.type == MOUSEBUTTONDOWN:                
                if event.button == 1:
                    main_menu = False
            
           




start_screen()

def score():
    
    if fish_caught_lib['trash'] > 0:
        poeng_trash = fish_caught_lib['trash']
        score_trash = myfont.render("TRASH {0}".format(poeng_trash), 1, (0, 0, 0))
        screen.blit(score_trash, (0, 10))
    
    if fish_caught_lib['rare'] > 0:
        poeng_rare = fish_caught_lib['rare']
        score_rare = myfont.render("RARE {0}".format(poeng_rare), 1, (0, 0, 0))
        screen.blit(score_rare, (0, 50))
    
    if fish_caught_lib['legendary'] > 0:
        poeng_legendary = fish_caught_lib['legendary']
        score_legendary = myfont.render("LEGENDARY {0}".format(poeng_legendary), 1, (0, 0, 0))
        screen.blit(score_legendary, (0, 90))

def throw_physics(t, v0, a=0):    
    s = v0 * t + 0.5 * a * t**2
    return s
        


def playerIdleAnimation():
    global idleAnimationCount
    if idleAnimationCount >= 24:
        idleAnimationCount = 0
    screen.blit(player[idleAnimationCount//3], player_pos) #deler på 3 pga 8 bilder og 24 fps.
    idleAnimationCount += 1
    pygame.display.update()
    
    


def fisk_minigame():
    key_lib = {49:'1', 50:'2', 51:'3', 52:'4', 53:'5'}       
    
    
    event_key = False
    lengde = 0
    height = 30
    width = 10
    
    x = 10
    y = 10
    r = 0
    g = 255
    t = 0
    dt = 0.1
    
    while lengde < 12 and event_key == False: #Når krafta på kastet og at man slipper SPACE
        ##########Throwbar power#########
        lengde = lengde + 1
        lengde_res = lengde                       
        time.sleep(0.2)
        
        
        ########Throwbar animation#########
        pygame.draw.rect(screen, (r,g,0), pygame.Rect(x, y, width, height))
        screen.blit(throwbar,(10,10))
        g -= 20
        r += 20
        playerIdleAnimation()
        if width <= 95:
            width = width + 10
        pygame.display.update()
        
        ##########Break loop when you let go of space##########
        for event in pygame.event.get():
            if event.type == KEYUP:
                if event.key == K_SPACE:
                    event_key = True
                    
                    while y >= -100:    
                        x = throw_physics(t, (lengde*8))
                        y = throw_physics(t, -30, 9.81)
                        x_dupp = player_pos[0] + x + 35
                        y_dupp = player_pos[1] + y + 5
                        t += dt
                        time.sleep(0.015)
                        dupp = pygame.draw.rect(screen, (12,15,23), pygame.Rect(x_dupp, y_dupp, 3 , 3))
                        
                        if y >= 300:
                            y = -101
                        pygame.display.update()
                        
                        
                    catch_timer = 1
                    fish_time = ra.randint(2,5)
                    screen_draw()
                    playerIdleAnimation()
        
        ###############Second part of minigame, catch timer###########
                    random_keys = ra.randint(49,53)
                    print("\n \n \n")
                    print("press the key:",key_lib[random_keys]) #just for game testing.
                    time.sleep(fish_time)
                    
                    while catch_timer > 0.1:                       
                        catch_timer -= 0.3
                        time.sleep(0.3)
                        
                        #Displays picure 1,2,3,4 or 5, depending on what you roll.
                        screen.blit(key_png[int(key_lib[random_keys])],(player_pos[0], player_pos[1] - 100 ))
                        
                        pygame.display.update()
                        for event in pygame.event.get():
                            if event.type == KEYDOWN:
                                if event.key == random_keys:
                                    return lengde
                                elif event.key != random_keys:
                                    lengde = 12
                                    return lengde
                    lengde = 12
                    return lengde
        #######################################################
        
                    


            
def type_fisk(rarity, fisk_res, fisk_draw, fisk_tekst):
    
    screen_draw()
    screen.blit(fisk_tekst, (player_pos[0] + 12, player_pos[1] - 70))
    pygame.display.update()
    playerIdleAnimation() 
    screen.blit(got_fish, player_pos)
    print("legendary: 0-10 \n"+"rare: 11-50 \n"+"trash: 51-100 \n")
    print("you rolled: ",fisk_res,"you got", power, "as a throw bonus!\n"+"you would normaly get", fisk_res + power)
    screen.blit(fisk_draw, (player_pos[0]+8 ,player_pos[1] - 40))
    pygame.display.update()
    time.sleep(2.5)
    
    
    
    
            
def kast(stang, snelle):
    fisk = ra.randint(stang, snelle)
    return fisk            
    

def fisk_result(power):
    fisk_res = (kast(stang, snelle) - power) #combines the players snelle and stang.
    print("\n \n")
    legendary = "LEGENDARY"
    rare = "RARE"
    trash = "TRASH"
    
   
    if fisk_res <= 10:
        type_fisk(legendary, fisk_res, fisk_legendary_1, legendary_text)
        fish_caught_lib['legendary'] += 1
       
        
    elif fisk_res > 10 and fisk_res < 51:
        type_fisk(rare, fisk_res, fisk_rare_1, rare_text)
        fish_caught_lib['rare'] += 1
        
    elif fisk_res >= 51:
        type_fisk(trash, fisk_res, fisk_trash_1, trash_text)
        fish_caught_lib['trash'] += 1
        
        
    
def screen_draw():
    screen.blit(background, (-150,-452))
    screen.blit(cloud_1, cloud_1_pos)
    screen.blit(dirt, (-140,310))
    screen.blit(sea_2, sea_2_pos)
    screen.blit(sea, sea_pos)
    
        

#####################################################
##################GAMELOOP###########################    
#####################################################
    
while True: 
        
    screen_draw()
          
    
    if moving_left == True and player_pos[0] >= -10:
        player_pos[0] -= 3   #If moving_left is True, move "player" 3 pixels to the left.
    
    if moving_right == True and player_pos[0] <= 260:
        player_pos[0] += 3   #Same as above but right.
    
    
    #CLOUD ANIMATION#
    cloud_1_pos[0] += 0.3
    if cloud_1_pos[0] >= 800:
        cloud_1_pos[0] = -550
        cloud_1_pos[1] = 0
        
    if cloud_1_pos[0] > 250:
        cloud_1_pos[1] += 0.01
    else:
        cloud_1_pos[1] -= 0.01
      
        
    #SEA ANIMATION#    
    sea_speed += 8
    if sea_speed >= 1200:
        sea_speed = 0
    
    if sea_speed <= 600:
        sea_pos[1] -= 0.15
    else:
        sea_pos[1] += 0.15

    if sea_speed <= 600:
        sea_2_pos[1] += 0.15
    else:
        sea_2_pos[1] -= 0.15
    
 
    
    for event in pygame.event.get():    #Check for events in the game window, eg: typing on keyboard.
        
        
        
        
        if event.type == QUIT:  #Exit Game function           
            pygame.quit()               
            sys.exit()  
    
            
        if event.type == KEYDOWN:   #Register key press
            if event.key == K_LEFT:
                moving_left = True  #If specified arrow is pressed down, make moving_direction True
            
            if event.key == K_RIGHT:
                moving_right = True
                
            if event.key == 99: #99 = key C
                print(fish_caught_lib)
           
                
            if event.key == K_SPACE:
                
                screen_draw()
                power = fisk_minigame()
                
                try:
                    if power <= 11:
                        fisk_result(power)
                except:
                    print("ohwell")
                
               
            ########SWAP FISHING GEAR#########
            if event.key == K_UP:    
                if stang == stang_1:
                    stang = stang_2
                    snelle = snelle_2
                    print("You switched to: TRASH ROD")
                    #switch_trash#
                    
                
                elif stang == stang_2:
                    stang = stang_1
                    snelle = snelle_1
                    print("You switched to: LEGENDARY ROD")
                    #switch_legendary#
                    
                    
                
        
        if event.type == KEYUP:     #Register key release
            if event.key == K_LEFT:
                moving_left = False
            
            if event.key == K_RIGHT:
                moving_right = False
                
        
                
                
    
        
            
        
        
        
    score()   
    fish_caught_lib.update()    
    playerIdleAnimation()    
    pygame.display.update() #Refresh the display/game window.
    clock.tick(24) #make the game 24 fps
  













#### IDEER ####

 #if event.type == MOUSEMOTION:
                #print(event.pos)
             #   if event.pos[0] >= 300 and event.pos[0] <= 500 and event.pos[1] <= 250 and event.pos[1] >= 100:
              #      print("TESTING")
               #     if event.type == MOUSEBUTTONDOWN:
                #        if event.button == 1:
                 #           main_menu = False
                #time.sleep(0.1)
                #if event.pos[0] <= 300:
                    #mouse_pos = False
                #time.sleep(0.5)
            #print(event.type)          
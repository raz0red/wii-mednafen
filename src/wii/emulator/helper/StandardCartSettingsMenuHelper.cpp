#include "StandardCartSettingsMenuHelper.h"
#include "StandardDatabaseManager.h"

#include "gettext.h"

#include "wii_app.h"
#include "wii_sdl.h"
#include "wii_mednafen.h"

StandardCartSettingsMenuHelper::StandardCartSettingsMenuHelper( 
  Emulator& emulator ) :
  CartridgeSettingsMenuHelper( emulator ),
  m_currentController( 0 ),
  m_currentProfile( 0 )
{
}

void StandardCartSettingsMenuHelper::addProfileNode( TREENODE* parent )
{
  TREENODE* child = wii_create_tree_node( NODETYPE_PROFILE, "Profile" );
  wii_add_child( parent, child );
}

void StandardCartSettingsMenuHelper::addControllerNode( TREENODE* parent )
{
  TREENODE* child = wii_create_tree_node( NODETYPE_CONTROLLER, "Controller" );
  wii_add_child( parent, child );
}

void StandardCartSettingsMenuHelper::addWiimoteSupportedNode( TREENODE* parent )
{
  TREENODE* child = wii_create_tree_node( 
    NODETYPE_WIIMOTE_SUPPORTED, "Supported" );
  wii_add_child( parent, child );
}

void StandardCartSettingsMenuHelper::addButtonMappingNodes( TREENODE* parent )
{
  int button;
  for( button = NODETYPE_BUTTON1; button <= NODETYPE_BUTTON10; button++ )
  {
    TREENODE* child = wii_create_tree_node( (NODETYPE)button, "Button" );
    wii_add_child( parent, child );
  }
}

void StandardCartSettingsMenuHelper::getNodeName( 
  TREENODE* node, char *buffer, char* value )
{
  CartridgeSettingsMenuHelper::getNodeName( node, buffer, value );

  Emulator& emu = getEmulator();
  StandardDatabaseManager& dbManager = 
    (StandardDatabaseManager&)emu.getDbManager();
  StandardDbEntry* entry = (StandardDbEntry*)dbManager.getEntry();
  
  switch( node->node_type )
  {
    case NODETYPE_WIIMOTE_SUPPORTED:
      snprintf( value, WII_MENU_BUFF_SIZE, "%s",
        ( entry->base.wiimoteSupported ?  "Yes" : "No" ) );
      break;
    case NODETYPE_CONTROLLER:
      snprintf( value, WII_MENU_BUFF_SIZE, "%s",
        StandardDatabaseManager::WII_CONTROLLER_NAMES[m_currentController] );
      break;
    case NODETYPE_PROFILE:
      snprintf( value, WII_MENU_BUFF_SIZE, "%s",
        dbManager.getProfileName( m_currentProfile ) );
      break;
    case NODETYPE_BUTTON1:
    case NODETYPE_BUTTON2:
    case NODETYPE_BUTTON3:
    case NODETYPE_BUTTON4:
    case NODETYPE_BUTTON5:
    case NODETYPE_BUTTON6:
    case NODETYPE_BUTTON7:
    case NODETYPE_BUTTON8:
    case NODETYPE_BUTTON9:
    case NODETYPE_BUTTON10:
      {
        int index = ( node->node_type - NODETYPE_BUTTON1 );
        const char* name = 
          dbManager.getMappedButton( 
            m_currentProfile, m_currentController, index )->name;
        if( name != NULL )
        {
          snprintf( buffer, WII_MENU_BUFF_SIZE, "%s", name );
          u8 btn = 
            entry->buttonMap[m_currentProfile][m_currentController][index];
          const MappableButton* mappedBtn = 
            dbManager.getMappableButton( m_currentProfile, btn ); 
          const char* name = mappedBtn->name;
          u32 val = mappedBtn->button;
          
          char btnName[WII_MENU_BUFF_SIZE];
          if( val & BTN_RAPID )
          {
            snprintf( btnName, sizeof(btnName), "%s-%s",
              gettextmsg(name), gettextmsg("Rapid") );
          }
          else
          {
            Util_strlcpy( btnName, gettextmsg(name), sizeof(btnName) );
          }

          const char* desc = 
            ( btn != 0 ? entry->buttonDesc[m_currentProfile][btn] : "" );
          if( desc[0] != '\0' )
          {
            snprintf( value, WII_MENU_BUFF_SIZE, "%s (%s)", 
              btnName, gettextmsg(desc) );
          }
          else
          {
            snprintf( value, WII_MENU_BUFF_SIZE, "%s", btnName );
          }
        }
      }
      break;     
  }
}

void StandardCartSettingsMenuHelper::selectNode( TREENODE* node )
{
  CartridgeSettingsMenuHelper::selectNode( node );

  char buff[WII_MAX_PATH];

  Emulator& emu = getEmulator();
  StandardDatabaseManager& dbManager = 
    (StandardDatabaseManager&)emu.getDbManager();
  StandardDbEntry* entry = (StandardDbEntry*)dbManager.getEntry();

  LOCK_RENDER_MUTEX();

  switch( node->node_type )
  {
    case NODETYPE_CARTRIDGE_SETTINGS_CONTROLS:
      m_currentProfile = entry->profile; 
      // Add prior to viewing controls
      dbManager.addRewindButtons();       
      break;
    case NODETYPE_WIIMOTE_SUPPORTED:
      entry->base.wiimoteSupported ^= 1;
      break;
    case NODETYPE_PROFILE:
      {
        while( true )
        {
          m_currentProfile++; 
          if( m_currentProfile >= dbManager.getProfileCount() )
          {
            m_currentProfile = 0;
          }

          if( dbManager.isProfileAvailable( m_currentProfile ) )
          {
            break;
          }
        }
      }
      break;
    case NODETYPE_CONTROLLER:
      m_currentController++; 
      if( m_currentController >= WII_CONTROLLER_COUNT )
      {
        m_currentController = 0;
      }
      break;
    case NODETYPE_BUTTON1:
    case NODETYPE_BUTTON2:
    case NODETYPE_BUTTON3:
    case NODETYPE_BUTTON4:
    case NODETYPE_BUTTON5:
    case NODETYPE_BUTTON6:
    case NODETYPE_BUTTON7:
    case NODETYPE_BUTTON8:
    case NODETYPE_BUTTON9:
    case NODETYPE_BUTTON10:
      {
        int index = ( node->node_type - NODETYPE_BUTTON1 );
        const char* name = 
          dbManager.getMappedButton( 
            m_currentProfile, m_currentController, index )->name;
        if( name != NULL )
        {
          u8 mappedBtn =  
            entry->buttonMap[m_currentProfile][m_currentController][index];

          mappedBtn++;
          if( mappedBtn >= dbManager.getMappableButtonCount( m_currentProfile ) )
          {
            mappedBtn = 0;
          }

          entry->buttonMap[m_currentProfile][m_currentController][index] 
            = mappedBtn;
        }
      }
      break;     
  }

  UNLOCK_RENDER_MUTEX();
}

bool StandardCartSettingsMenuHelper::isNodeVisible( TREENODE* node )
{
  if( !CartridgeSettingsMenuHelper::isNodeVisible( node ) )
  {
    return false;
  }

  Emulator& emu = getEmulator();
  StandardDatabaseManager& dbManager = 
    (StandardDatabaseManager&)emu.getDbManager();
  StandardDbEntry* entry = (StandardDbEntry*)dbManager.getEntry();

  switch( node->node_type )
  {
    case NODETYPE_WIIMOTE_SUPPORTED:
      return m_currentController == WII_CONTROLLER_MOTE;
    case NODETYPE_BUTTON1:
    case NODETYPE_BUTTON2:
    case NODETYPE_BUTTON3:
    case NODETYPE_BUTTON4:
    case NODETYPE_BUTTON5:
    case NODETYPE_BUTTON6:
    case NODETYPE_BUTTON7:
    case NODETYPE_BUTTON8:
    case NODETYPE_BUTTON9:
    case NODETYPE_BUTTON10:
      return 
        dbManager.getMappedButton( 
            m_currentProfile, m_currentController, 
              node->node_type - NODETYPE_BUTTON1 )->name != NULL;
  }

  return true;
}

void StandardCartSettingsMenuHelper::setCurrentProfile( int profile )
{
  m_currentProfile = profile;
}
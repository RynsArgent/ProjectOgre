#include "renderer.h"

#include "character.h"
#include "formation.h"
#include "ability.h"
#include "unit.h"
#include "group.h"
#include "battle.h"
#include "event.h"
#include "action.h"
#include <sstream>
#include <cmath>
#include <vector>

extern bool change;

JobType jobs[] = { JOB_NONE, JOB_FIGHTER, JOB_SCOUT, JOB_ACOLYTE, JOB_MAGE };
static const int NUM_JOBS = sizeof(jobs) / sizeof(JobType);

bool hasName(Formation* formation, const string & value)
{
	for (int i = 0; i < formation->getWidth(); ++i)
		for (int j = 0; j < formation->getHeight(); ++j)
		{
			Character* character = formation->getCharacterAt(i, j);
			if (character && character->getName() == value)
				return true;
		}
	return false;
}

string getAvailableName(bool form, Formation* formation, JobType job)
{
	string body = toStringJob(job);

	for (int s = 1; s <= 9; ++s) {
		string test = body + toStringInt(s);
		if (form) test += 'B';
		else test += 'A';

		if (!hasName(formation, test))
			return test;
	}
	return "null";
}

void InfoBox::setHighlighted(bool value)
{
	highlighted = value;
}

void InfoBox::renderOutline(const Color & col) const
{
	Rect2D(box.p, box.width, box.height, col, false).render();
}

void InfoBox::render(Renderer* renderer) const
{
	box.render();
	if (highlighted) 
		renderOutline(YELLOW);
	if (renderer->selected == this)
		renderOutline(RED);
}

void TextInfoBox::render(Renderer* renderer) const
{
	InfoBox::render(renderer);
	
	renderer->GLoutputString(box.center(), text, textColor, true);
}

void JobInfoBox::render(Renderer* renderer) const
{
	InfoBox::render(renderer);
	
	renderer->GLoutputString(box.center(), toStringJob(info), textColor, true);
}

void CharacterInfoBox::render(Renderer* renderer) const
{
	InfoBox::render(renderer);

	if (info != NULL) {
		Color colText = BLUE;

		string descName = info->getName();
		renderer->GLoutputString(Point2D(box.p.x + box.width / 2, box.p.y + 3 * (box.height / 10)), descName, colText, true);\
		if (leader)
			renderer->GLoutputString(Point2D(box.p.x + box.width / 2, box.p.y + 3 * (2 * box.height / 10)), "***", colText, true);
	}
	
	if (renderer->selected == this || (renderer->mouseover == this && renderer->selected == NULL)) {
		SideboardInfoBox* sideboard = &renderer->setupInfo.sideboardInfo;
		sideboard->render(renderer);
	}
}

void UnitInfoBox::render(Renderer* renderer) const
{
	if (info != NULL && info->isAvailable()) {
		InfoBox::render(renderer);

		double barWidth = box.width;
		double barHeight = box.height / 4;
		double percentHealth = static_cast<double>(info->getCurrentHealth()) / info->getMaxHealth();
		double scaledWidth = barWidth * percentHealth;

		glBegin(GL_QUADS);
		if (percentHealth >= 0.5)
			glColor4d(1 - (percentHealth - 0.5) / 0.5, 1.0, 0.0, 0.0);
		else
			glColor4d(1.0, percentHealth / 0.5, 0.0, 0.0);
		glVertex2d(box.p.x, box.p.y);
		glVertex2d(box.p.x + scaledWidth, box.p.y);
		glVertex2d(box.p.x + scaledWidth, box.p.y + barHeight);
		glVertex2d(box.p.x, box.p.y + barHeight);
		glEnd();

		Color colText;
		if (renderer->battleInfo.info->getMainUnit() == info)
			colText = Color(0.0, 1.0, 0.0);
		else
		{
			if (info->isDone())
				colText = Color(0.2, 0.2, 0.2);
			else
				colText = Color(0.0, 0.0, 1.0);
		}
		string descLeader = info->isLeader() ? "***" : "";
		string descHP = toStringInt(info->getCurrentHealth()) + "/" + toStringInt(info->getMaxHealth());
		string descName = info->getName();
		renderer->GLoutputString(Point2D(box.p.x + max(barWidth * 0.30, scaledWidth) / 2, box.p.y + barHeight / 2), descLeader, colText, true);
		renderer->GLoutputString(Point2D(box.p.x + box.width / 2, box.p.y + 1.5 * barHeight), descHP, colText, true);
		renderer->GLoutputString(Point2D(box.p.x + box.width / 2, box.p.y + 3.0 * barHeight), descName, colText, true);
	}
}

void FormationInfoBox::render(Renderer* renderer) const
{
	InfoBox::render(renderer);
	for (int i = 0; i < characterInfos.size(); ++i)
		for (int j = 0; j < characterInfos[i].size(); ++j)
		{
			characterInfos[i][j].render(renderer);
		}
	targetInfo.render(renderer);
}

void GroupInfoBox::render(Renderer* renderer) const
{
	InfoBox::render(renderer);
	for (int i = 0; i < unitInfos.size(); ++i)
		for (int j = 0; j <unitInfos[i].size(); ++j)
		{
			unitInfos[i][j].render(renderer);
		}
}

void SideboardInfoBox::render(Renderer* renderer) const
{
	InfoBox::render(renderer);
	
	if (info != NULL) {
		for (int i = 0; i < skillboxes.size(); ++i)
			for (int j = 0; j < skillboxes[i].size(); ++j)
				skillboxes[i][j].render(renderer);

		for (int i = 0; i < elementboxes.size(); ++i)
			elementboxes[i].render(renderer);
	}

	for (int i = 0; i < jobboxes.size(); ++i)
	for (int j = 0; j < jobboxes[i].size(); ++j)
		jobboxes[i][j].render(renderer);
	leftbox.render(renderer);
	rightbox.render(renderer);
}

void SetupInfoBox::render(Renderer* renderer) const
{
	InfoBox::render(renderer);
	formAInfo.render(renderer);
	formBInfo.render(renderer);
	playBox.render(renderer);
}

void BattleInfoBox::render(Renderer* renderer) const
{
	InfoBox::render(renderer);
	groupAInfo.render(renderer);
	groupBInfo.render(renderer);

	vector<Event*> eventStack = info->getEventStack();
	for (int i = 0; i < eventStack.size(); ++i) {
		stringstream ss;
		eventStack[i]->print(ss);
		renderer->GLoutputString(0, 0.05 + 0.05 * i, ss.str(), 1.0, 1.0, 1.0, GLUT_BITMAP_HELVETICA_12);
	}
	renderer->GLoutputString(0.01, 0.99, "seed: " + toStringInt(info->getSeed()), 1.0, 1.0, 1.0, GLUT_BITMAP_HELVETICA_12);
}

Renderer::Renderer(int pixelWidth, int pixelHeight, double edgeLeft, double edgeRight, double edgeTop, double edgeBottom)
	: pixelWidth(pixelWidth), pixelHeight(pixelHeight), edgeLeft(edgeLeft), edgeRight(edgeRight), edgeTop(edgeTop), edgeBottom(edgeBottom),
	selected(NULL), mouseover(NULL)
{
}

//Outputs a string in graphics to point p with str and color (r,g,b) and font
void Renderer::GLoutputString(double x, double y, const string & str, 
								double cr, double cg, double cb, bool centered)
{
	glColor3d(cr, cg, cb);
	if (centered) {
		
		double totalWidth = 0;
		double totalHeight = 18;
		for (unsigned i = 0; i < str.length(); i++) 
			totalWidth += glutBitmapWidth(GLUT_BITMAP_HELVETICA_18, str[i]);
		totalWidth *= (width() / pixelWidth);
		totalHeight *= (height() / pixelHeight);
		glRasterPos2f(x - totalWidth / 2, y + totalHeight / 2);

	}
	else
		glRasterPos2f(x, y);
	for (unsigned i = 0; i < str.length(); i++) 
	{
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, str[i]);
	}
}

void Renderer::GLoutputString(const Point2D & p, const string & str, 
								const Color & col, bool centered)
{
	glColor3d(col.r, col.g, col.b);
	if (centered) {
		
		double totalWidth = 0;
		double totalHeight = 18;
		for (unsigned i = 0; i < str.length(); i++) {
			totalWidth += glutBitmapWidth(GLUT_BITMAP_HELVETICA_18, str[i]);
		}
		totalWidth *= (width() / pixelWidth);
		totalHeight *= (height() / pixelHeight);
		glRasterPos2f(p.x - totalWidth / 2, p.y + totalHeight / 2);

	}
	else
		glRasterPos2f(p.x, p.y);
	for (unsigned i = 0; i < str.length(); i++) 
	{
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, str[i]);
	}
}

///////////////////////
///	Setup Functions ///
///////////////////////
void Renderer::setSideboardBox(SideboardInfoBox* container, Character* character)
{
	container->info = character;
	/*
	if (character == NULL) {
		container->skillboxes = vector<vector<TextInfoBox> >();
		container->elementboxes = vector<InfoBox>();
		container->jobboxes = vector<vector<JobInfoBox> >();
		return;
	}
	*/
	
	Rect2D sbox = container->box;
	Rect2D skillbox = Rect2D(
		sbox.p, 
		sbox.width, 
		2 * sbox.height / 5, 
		WHITE, true);
	Rect2D elementbox = Rect2D(
		sbox.p + Point2D(0, 2 * sbox.height / 5), 
		sbox.width, sbox.height / 10, 
		WHITE, true);
	Rect2D classbox = Rect2D(
		elementbox.p + Point2D(0, elementbox.height), 
		sbox.width, 
		sbox.height / 2,
		WHITE, true);

	const double padding = 0.005;
	
	const double descWidth = skillbox.width / 5;
	const double descHeight = skillbox.height;
	const double skillWidth = skillbox.width - descWidth;
	const double skillHeight = descHeight;
	
	const double modDescWidth = descWidth - 2 * padding;
	const double modDescHeight = descHeight - padding * (container->SB_HEIGHT + 1);
	const double modSkillWidth = skillWidth - padding * (container->SB_WIDTH + 1);
	const double modSkillHeight = skillHeight - padding * (container->SB_HEIGHT + 1);

	const double dSkillWidth = modSkillWidth / (container->SB_WIDTH - 1) + padding;
	const double dSkillHeight = modSkillHeight / container->SB_HEIGHT + padding;
	
	container->skillboxes = vector<vector<TextInfoBox> >(
		container->SB_WIDTH, vector<TextInfoBox>(
		container->SB_HEIGHT));
	container->elementboxes = vector<InfoBox>(container->ELE_WIDTH);
	container->jobboxes = vector<vector<JobInfoBox> >(
		container->JOB_WIDTH, vector<JobInfoBox>(
		container->JOB_HEIGHT));
	
	for (int j = 0; j < container->SB_HEIGHT; ++j)
		container->skillboxes[0][j].box = Rect2D(skillbox.p + Point2D(padding, padding + j * dSkillHeight), 
				modDescWidth, modDescHeight / container->SB_HEIGHT, WHITE, true);

	for (int i = 1; i < container->SB_WIDTH; ++i)
		for (int j = 0; j < container->SB_HEIGHT; ++j)
		{
			if (j == 3 && i > 0)
				continue;

			container->skillboxes[i][j].box = Rect2D(skillbox.p + Point2D(descWidth + padding + (i - 1) * dSkillWidth, padding + j * dSkillHeight), 
				modSkillWidth / (container->SB_WIDTH - 1), 
				modSkillHeight / container->SB_HEIGHT, 
				WHITE, true);

			if (character) {
				switch (j) {
					case 0:
						if (character->getBackSkillIndex() == i - 1)	
							container->skillboxes[i][j].box.color = GREEN;	
						break;
					case 1:
						if (character->getMidSkillIndex() == i - 1)
							container->skillboxes[i][j].box.color = GREEN;	
						break;
					case 2:
						if (character->getFrontSkillIndex() == i - 1)
							container->skillboxes[i][j].box.color = GREEN;	
						break;
				}
			}
		}

	container->skillboxes[1][3].box = Rect2D(skillbox.p + Point2D(descWidth + padding, padding + 3 * dSkillHeight), 
		2 * modSkillWidth / (container->SB_WIDTH - 1) + padding, 
		modSkillHeight / container->SB_HEIGHT, 
		WHITE, true);
	container->skillboxes[1][3].box.color = GREEN;	
	
	container->skillboxes[0][0].text = "Back:";
	container->skillboxes[0][1].text = "Middle:";
	container->skillboxes[0][2].text = "Front:";
	container->skillboxes[0][3].text = "Basic:";
	if (character) {
		container->skillboxes[1][0].text = Ability::toStringSkill(character->getBackSkills()[0]);
		container->skillboxes[2][0].text = Ability::toStringSkill(character->getBackSkills()[1]);
		container->skillboxes[1][1].text = Ability::toStringSkill(character->getMidSkills()[0]);
		container->skillboxes[2][1].text = Ability::toStringSkill(character->getMidSkills()[1]);
		container->skillboxes[1][2].text = Ability::toStringSkill(character->getFrontSkills()[0]);
		container->skillboxes[2][2].text = Ability::toStringSkill(character->getFrontSkills()[1]);
		container->skillboxes[1][3].text = Ability::toStringSkill(character->getBasicSkills()[0]);
	}
	for (int i = 0; i < container->SB_WIDTH; ++i)
		for (int j = 0; j < container->SB_HEIGHT; ++j)
			container->skillboxes[i][j].textColor = BLUE;

	const double eleWidth = elementbox.width;
	const double eleHeight = elementbox.height;
	
	const double modEleWidth = eleWidth - padding * (container->ELE_WIDTH + 2);
	const double modEleHeight = eleHeight - 2 * padding;
	
	const double dEleWidth = modEleWidth / (container->ELE_WIDTH) + padding;
	for (int i = 0; i < container->ELE_WIDTH; ++i)
	{
		container->elementboxes[i].box = Rect2D(elementbox.p + Point2D(padding + i * dEleWidth, padding), 
				modEleWidth / container->ELE_WIDTH, 
				modEleHeight, 
				WHITE, true);
	}

	if (character) {
		switch (character->getFavoredElement()) {
			case ELEMENT_PHYSICAL:
				container->elementboxes[0].box.color = GRAY;
				break;
			case ELEMENT_FIRE:
				container->elementboxes[1].box.color = RED;
				break;
			case ELEMENT_WATER:
				container->elementboxes[2].box.color = BLUE;
				break;
			case ELEMENT_EARTH:
				container->elementboxes[3].box.color = GREEN;
				break;
			case ELEMENT_ICE:
				container->elementboxes[4].box.color = CYAN;
				break;
			case ELEMENT_LIGHTNING:
				container->elementboxes[5].box.color = YELLOW;
				break;
		}
	}

	const double jobWidth = 4 * classbox.width / 5;
	const double jobHeight = classbox.height;
	const double scrollWidth = classbox.width - jobWidth;
	const double scrollHeight = jobHeight;
	
	const double modJobWidth = jobWidth - 2 * padding;
	const double modJobHeight = jobHeight - padding * (container->JOB_HEIGHT + 1);
	const double modScrollWidth = scrollWidth - 2 * padding;
	const double modScrollHeight = scrollHeight - 3 * padding;
	
	const double dJobWidth = modJobWidth / (container->JOB_WIDTH) + padding;
	const double dJobHeight = modJobHeight / (container->JOB_HEIGHT) + padding;
	int jc = container->jobindex * container->JOB_WIDTH * container->JOB_HEIGHT;
	for (int i = 0; i < container->JOB_WIDTH; ++i)
		for (int j = 0; j < container->JOB_HEIGHT; ++j)
		{
			container->jobboxes[i][j].box = Rect2D(classbox.p + Point2D(padding + i * dJobWidth, padding + j * dJobHeight), 
								modJobWidth / container->JOB_WIDTH, modJobHeight / container->JOB_HEIGHT, WHITE, true);	
			if (jc < NUM_JOBS) {
				container->jobboxes[i][j].info = jobs[jc];
				container->jobboxes[i][j].textColor = BLACK;
			} else {
				container->jobboxes[i][j].box.color = GRAY;
				container->jobboxes[i][j].textColor = GRAY;
			}

			++jc;
		}
	
	const double dScrollHeight = modScrollHeight / 2 + padding;
	container->leftbox.box = Rect2D(classbox.p + Point2D(jobWidth + padding, padding), modScrollWidth, modScrollHeight / 2, WHITE, true);
	container->rightbox.box = Rect2D(classbox.p + Point2D(jobWidth + padding, padding + dScrollHeight), modScrollWidth, modScrollHeight / 2, WHITE, true);
	container->leftbox.text = "<<";
	container->rightbox.text = ">>";
	container->leftbox.textColor = BLACK;
	container->rightbox.textColor = BLACK;
}

void Renderer::setCharacterBox(CharacterInfoBox* container, Character* character, double centerx, double centery, double width, double height)
{
	container->info = character;
	double xpos = centerx - width / 2;
	double ypos = centery - height / 2;
	container->box = Rect2D(Point2D(xpos, ypos), width, height, WHITE, true);
}

void Renderer::setFormationBox(FormationInfoBox* container, Formation* form, Direction dir, double centerx, double centery, double width, double height)
{
	container->info = form;

	double dx = width / (form->getWidth());
	double dy = height / (form->getHeight());
	double unitwidth = dx / 1.20;
	double unitheight = dy / 1.20;
	double startingx = centerx - width / 2;
	double startingy = centery - height / 2;

	container->box = Rect2D(Point2D(startingx, startingy), width, height);
	if (dir == DIRECTION_SOUTH)
		container->box.color = Color(1.0, 0.5, 0.5);
	else if (dir == DIRECTION_NORTH)
		container->box.color = Color(0.5, 0.5, 1.0);
	container->box.filled = true;
	
	container->characterInfos = vector<vector<CharacterInfoBox> >(form->getWidth(), vector<CharacterInfoBox>(form->getHeight()));
	for (int i = 0; i < form->getWidth(); ++i)
		for (int j = 0; j < form->getHeight(); ++j)
		{
			// can be null
			Character* character = form->getCharacterAt(i, j);

			int xmod;
			int ymod;
			switch (dir)
			{
				case DIRECTION_NORTH:
					xmod = i;
					ymod = j;
					break;
				case DIRECTION_WEST:
					xmod = j;
					ymod = form->getHeight() - i - 1;
					break;
				case DIRECTION_EAST:
					xmod = form->getWidth() - j - 1;
					ymod = i;
					break;
				case DIRECTION_SOUTH:
					xmod = form->getWidth() - i - 1;
					ymod = form->getHeight() - j - 1;
					break;
				default:
					xmod = i;
					ymod = j;
					break;
			}
			double xpos = startingx + xmod * dx + dx / 2;
			double ypos = startingy + ymod * dy + dx / 2;
			setCharacterBox(&container->characterInfos[i][j], character, xpos, ypos, unitwidth, unitheight);
			container->characterInfos[i][j].x = i;
			container->characterInfos[i][j].y = j;
			container->characterInfos[i][j].form = dir == DIRECTION_SOUTH;
			container->characterInfos[i][j].leader = form->getLeader() == character;
		}
		
	container->targetInfo.box = Rect2D(container->box.p + Point2D(container->box.width + dx / 8, 0.0),
		dx / 5.0, dy / 4.0, WHITE, true);
	switch (form->getTargetOrder()) 
	{
	case TARGET_RANDOM:
		container->targetInfo.text = "R";
		break;
	case TARGET_WEAKEST:
		container->targetInfo.text = "W";
		break;
	case TARGET_STRONGEST:
		container->targetInfo.text = "S";
		break;
	case TARGET_LEADER:
		container->targetInfo.text = "L";
		break;
	default:
		container->targetInfo.text = "?";
		break;
	}
	container->targetInfo.textColor = BLACK;
}

void Renderer::initSetupRenderer(Setup* setup)
{
	setupInfo.info = setup;

	double w = (edgeRight - edgeLeft) / 2;
	double h = (edgeBottom - edgeTop);
	setupInfo.sideboardInfo.box = Rect2D(Point2D(0.0, 0.0), w, h, BLACK, true);
	setupInfo.box = Rect2D(Point2D((edgeLeft + edgeRight) / 2, edgeTop), w, h, GRAY, true); 
	
	setFormationBox(&setupInfo.formAInfo, setup->formA, DIRECTION_NORTH, 0.75, 0.75, 0.40, 0.40);
	setFormationBox(&setupInfo.formBInfo, setup->formB, DIRECTION_SOUTH, 0.75, 0.25, 0.40, 0.40);
	
	setupInfo.playBox.box.width = setupInfo.box.width / 8;
	setupInfo.playBox.box.height = setupInfo.box.height / 25;
	setupInfo.playBox.box.p = setupInfo.box.center() - Point2D(setupInfo.playBox.box.width, setupInfo.playBox.box.height) / 2;
	setupInfo.playBox.box.color = WHITE;
	setupInfo.playBox.box.filled = true;
	setupInfo.playBox.text = "Play";
	setupInfo.playBox.textColor = BLACK;
}

void Renderer::renderSetup()
{
	setupInfo.render(this);
}

void Renderer::processMouseLeftClickSetup(const Point2D & loc)
{
	if (setupInfo.sideboardInfo.info != NULL)
	{
		for (int i = 1; i < setupInfo.sideboardInfo.skillboxes.size(); ++i)
			for (int j = 0; j < setupInfo.sideboardInfo.skillboxes[i].size(); ++j)
			{
				if (setupInfo.sideboardInfo.skillboxes[i][j].box.contains(loc))
				{
					switch (j) {
						case 0:
							setupInfo.sideboardInfo.info->setBackSkillIndex(i - 1);
							break;
						case 1:
							setupInfo.sideboardInfo.info->setMidSkillIndex(i - 1);
							break;
						case 2:
							setupInfo.sideboardInfo.info->setFrontSkillIndex(i - 1);
							break;
					}
					setSideboardBox(&setupInfo.sideboardInfo, setupInfo.sideboardInfo.info);
				}
			}
			
		for (int i = 0; i < setupInfo.sideboardInfo.elementboxes.size(); ++i)
		{
			if (setupInfo.sideboardInfo.elementboxes[i].box.contains(loc))
			{
				switch (i) {
				case 0:
					setupInfo.sideboardInfo.info->setFavoredElement(ELEMENT_PHYSICAL);
					break;
				case 1:
					setupInfo.sideboardInfo.info->setFavoredElement(ELEMENT_FIRE);
					break;
				case 2:
					setupInfo.sideboardInfo.info->setFavoredElement(ELEMENT_WATER);
					break;
				case 3:
					setupInfo.sideboardInfo.info->setFavoredElement(ELEMENT_EARTH);
					break;
				case 4:
					setupInfo.sideboardInfo.info->setFavoredElement(ELEMENT_ICE);
					break;
				case 5:
					setupInfo.sideboardInfo.info->setFavoredElement(ELEMENT_LIGHTNING);
					break;
				}
				setupInfo.sideboardInfo.info->updateJobInfo();
				setSideboardBox(&setupInfo.sideboardInfo, setupInfo.sideboardInfo.info);
			}
		}
	}

	for (int i = 0; i < setupInfo.sideboardInfo.jobboxes.size(); ++i)
		for (int j = 0; j < setupInfo.sideboardInfo.jobboxes[i].size(); ++j)
		{
			if (setupInfo.sideboardInfo.jobboxes[i][j].box.contains(loc))
			{
				JobType newjob = setupInfo.sideboardInfo.jobboxes[i][j].info;

				CharacterInfoBox* characterinfo = static_cast<CharacterInfoBox*>(selected);
				int x = characterinfo->x;
				int y = characterinfo->y;
				FormationInfoBox* form = characterinfo->form ? &setupInfo.formBInfo : &setupInfo.formAInfo;
				
				if (form->info->getCharacterAt(x, y) != NULL)
					delete form->info->getCharacterAt(x, y);
				form->info->setCharacterAt(x, y, NULL, 0, 0, 0);
				characterinfo->info = NULL;

				if (newjob != JOB_NONE) {
					Character* newchar = new Character(getAvailableName(characterinfo->form, form->info, newjob), newjob);
					form->info->setCharacterAt(x, y, newchar, 0, 0, 0);
					characterinfo->info = newchar;
				}
			}
		}
		
	if (setupInfo.sideboardInfo.leftbox.box.contains(loc))
	{
		--setupInfo.sideboardInfo.jobindex;
		if (setupInfo.sideboardInfo.jobindex < 0)
			setupInfo.sideboardInfo.jobindex = NUM_JOBS / (SideboardInfoBox::JOB_WIDTH * SideboardInfoBox::JOB_HEIGHT);

	}
	
	if (setupInfo.sideboardInfo.rightbox.box.contains(loc))
	{
		++setupInfo.sideboardInfo.jobindex;
		if (setupInfo.sideboardInfo.jobindex * SideboardInfoBox::JOB_WIDTH * SideboardInfoBox::JOB_HEIGHT >= NUM_JOBS)
			setupInfo.sideboardInfo.jobindex = 0;
	}

	for (int i = 0; i < setupInfo.formAInfo.characterInfos.size(); ++i)
		for (int j = 0; j < setupInfo.formAInfo.characterInfos[i].size(); ++j)
		{
			CharacterInfoBox* characterinfo = &setupInfo.formAInfo.characterInfos[i][j];
			if (characterinfo->box.contains(loc))
			{
				if (selected == characterinfo)
					selected = NULL;
				else
					selected = characterinfo;
			}
		}

	for (int i = 0; i < setupInfo.formBInfo.characterInfos.size(); ++i)
		for (int j = 0; j < setupInfo.formBInfo.characterInfos[i].size(); ++j)
		{
			CharacterInfoBox* characterinfo = &setupInfo.formBInfo.characterInfos[i][j];
			if (characterinfo->box.contains(loc))
			{
				if (selected == characterinfo) 
					selected = NULL;
				else
					selected = characterinfo;
			}
		}

	if (setupInfo.playBox.box.contains(loc))
	{
		setupInfo.done = true;
	}

	if (setupInfo.formAInfo.targetInfo.box.contains(loc))
	{
		cout << "rawr";
		TargetType order = setupInfo.formAInfo.info->getTargetOrder();
		switch (order) 
		{
		case TARGET_RANDOM:
			order = TARGET_WEAKEST;
			setupInfo.formAInfo.targetInfo.text = "W";
			break;
		case TARGET_WEAKEST:
			order = TARGET_STRONGEST;
			setupInfo.formAInfo.targetInfo.text = "S";
			break;
		case TARGET_STRONGEST:
			order = TARGET_LEADER;
			setupInfo.formAInfo.targetInfo.text = "L";
			break;
		case TARGET_LEADER:
			order = TARGET_RANDOM;
			setupInfo.formAInfo.targetInfo.text = "R";
			break;
		default:
			setupInfo.formAInfo.targetInfo.text = "?";
			break;
		}
		setupInfo.formAInfo.info->setTargetOrder(order);
	}

	if (setupInfo.formBInfo.targetInfo.box.contains(loc))
	{
		TargetType order = setupInfo.formBInfo.info->getTargetOrder();
		switch (order) 
		{
		case TARGET_RANDOM:
			order = TARGET_WEAKEST;
			setupInfo.formBInfo.targetInfo.text = "W";
			break;
		case TARGET_WEAKEST:
			order = TARGET_STRONGEST;
			setupInfo.formBInfo.targetInfo.text = "S";
			break;
		case TARGET_STRONGEST:
			order = TARGET_LEADER;
			setupInfo.formBInfo.targetInfo.text = "L";
			break;
		case TARGET_LEADER:
			order = TARGET_RANDOM;
			setupInfo.formBInfo.targetInfo.text = "R";
			break;
		default:
			setupInfo.formBInfo.targetInfo.text = "?";
			break;
		}
		setupInfo.formBInfo.info->setTargetOrder(order);
	}

	change = true;
	if (change) {
		if (selected)
			setSideboardBox(&setupInfo.sideboardInfo, static_cast<CharacterInfoBox*>(selected)->info);
		else if (mouseover)
			setSideboardBox(&setupInfo.sideboardInfo, static_cast<CharacterInfoBox*>(mouseover)->info);
		else
			setSideboardBox(&setupInfo.sideboardInfo, NULL);
	}
}

void Renderer::processMouseRightClickSetup(const Point2D & loc)
{
	for (int i = 0; i < setupInfo.formAInfo.characterInfos.size(); ++i)
		for (int j = 0; j < setupInfo.formAInfo.characterInfos[i].size(); ++j)
		{
			CharacterInfoBox* characterinfo = &setupInfo.formAInfo.characterInfos[i][j];
			if (characterinfo->box.contains(loc))
			{
				GridPoint old = setupInfo.formAInfo.info->getLeaderPosition();
				GridPoint next = GridPoint(i, j);
				setupInfo.formAInfo.characterInfos[old.x][old.y].leader = false;
				setupInfo.formAInfo.info->setLeaderPosition(next);
				setupInfo.formAInfo.characterInfos[next.x][next.y].leader = true;
			}
		}

	for (int i = 0; i < setupInfo.formBInfo.characterInfos.size(); ++i)
		for (int j = 0; j < setupInfo.formBInfo.characterInfos[i].size(); ++j)
		{
			CharacterInfoBox* characterinfo = &setupInfo.formBInfo.characterInfos[i][j];
			if (characterinfo->box.contains(loc))
			{
				GridPoint old = setupInfo.formBInfo.info->getLeaderPosition();
				GridPoint next = GridPoint(i, j);
				setupInfo.formBInfo.characterInfos[old.x][old.y].leader = false;
				setupInfo.formBInfo.info->setLeaderPosition(next);
				setupInfo.formBInfo.characterInfos[next.x][next.y].leader = true;
			}
		}

	change = true;
	if (change) {
		if (selected)
			setSideboardBox(&setupInfo.sideboardInfo, static_cast<CharacterInfoBox*>(selected)->info);
		else if (mouseover)
			setSideboardBox(&setupInfo.sideboardInfo, static_cast<CharacterInfoBox*>(mouseover)->info);
		else
			setSideboardBox(&setupInfo.sideboardInfo, NULL);
	}
}

void Renderer::processMouseMoveSetup(const Point2D & loc)
{
	if (setupInfo.sideboardInfo.info != NULL)
	{
		for (int i = 1; i < setupInfo.sideboardInfo.skillboxes.size(); ++i)
			for (int j = 0; j < setupInfo.sideboardInfo.skillboxes[i].size(); ++j)
			{
				TextInfoBox* skillinfo = &setupInfo.sideboardInfo.skillboxes[i][j];
				if (skillinfo->box.contains(loc))
				{
					skillinfo->setHighlighted(true);
					if (mouseover != skillinfo)
					{
						mouseover = skillinfo;
					}
				}
				else
				{
					skillinfo->setHighlighted(false);
					if (mouseover == skillinfo)
					{
						mouseover = NULL;
					}
				}
			}
			
		for (int i = 0; i < setupInfo.sideboardInfo.elementboxes.size(); ++i)
		{
			InfoBox* elementinfo = &setupInfo.sideboardInfo.elementboxes[i];
			if (elementinfo->box.contains(loc))
			{
				elementinfo->setHighlighted(true);
				if (mouseover != elementinfo)
				{
					mouseover = elementinfo;
				}
			}
			else
			{
				elementinfo->setHighlighted(false);
				if (mouseover == elementinfo) 
				{
					mouseover = NULL;
				}
			}
		}
	}	

	for (int i = 0; i < setupInfo.sideboardInfo.jobboxes.size(); ++i)
		for (int j = 0; j < setupInfo.sideboardInfo.jobboxes[i].size(); ++j)
		{
			JobInfoBox* jobinfo = &setupInfo.sideboardInfo.jobboxes[i][j];
			if (jobinfo->box.contains(loc))
			{
				jobinfo->setHighlighted(true);
				if (mouseover != jobinfo)
				{
					mouseover = jobinfo;
				}
			}
			else
			{
				jobinfo->setHighlighted(false);
				if (mouseover == jobinfo) 
				{
					mouseover = NULL;
				}
			}
		}

		TextInfoBox* upinfo = &setupInfo.sideboardInfo.leftbox;
		if (upinfo->box.contains(loc))
		{
			upinfo->setHighlighted(true);
			if (mouseover != upinfo)
			{
				mouseover = upinfo;
			}
		}
		else
		{
			upinfo->setHighlighted(false);
			if (mouseover == upinfo) 
			{
				mouseover = NULL;
			}
		}
		TextInfoBox* downinfo = &setupInfo.sideboardInfo.rightbox;
		if (downinfo->box.contains(loc))
		{
			downinfo->setHighlighted(true);
			if (mouseover != downinfo)
			{
				mouseover = downinfo;
			}
		}
		else
		{
			downinfo->setHighlighted(false);
			if (mouseover == downinfo) 
			{
				mouseover = NULL;
			}
		}
	

	for (int i = 0; i < setupInfo.formAInfo.characterInfos.size(); ++i)
		for (int j = 0; j < setupInfo.formAInfo.characterInfos[i].size(); ++j)
		{
			CharacterInfoBox* characterinfo = &setupInfo.formAInfo.characterInfos[i][j];
			if (characterinfo->box.contains(loc))
			{
				
				characterinfo->setHighlighted(true);
				if (mouseover != characterinfo) 
				{
					mouseover = characterinfo;
					if (selected == NULL)
						setSideboardBox(&setupInfo.sideboardInfo, characterinfo->info);
				}
			}
			else
			{
				characterinfo->setHighlighted(false);
				if (mouseover == characterinfo) 
				{
					mouseover = NULL;
					if (selected == NULL)
						setSideboardBox(&setupInfo.sideboardInfo, NULL);
				}
			}
		}
				
	for (int i = 0; i < setupInfo.formBInfo.characterInfos.size(); ++i)
		for (int j = 0; j < setupInfo.formBInfo.characterInfos[i].size(); ++j)
		{
			CharacterInfoBox* characterinfo = &setupInfo.formBInfo.characterInfos[i][j];
			if (characterinfo->box.contains(loc))
			{
				characterinfo->setHighlighted(true);
				if (mouseover != characterinfo) 
				{
					mouseover = characterinfo;
					if (selected == NULL)
						setSideboardBox(&setupInfo.sideboardInfo, characterinfo->info);
				}
			}
			else
			{
				characterinfo->setHighlighted(false);
				if (mouseover == characterinfo) 
				{
					mouseover = NULL;
					if (selected == NULL)
						setSideboardBox(&setupInfo.sideboardInfo, NULL);
				}
			}
		}

	if (setupInfo.playBox.box.contains(loc))
	{
		setupInfo.playBox.setHighlighted(true);
	}
	else
	{
		setupInfo.playBox.setHighlighted(false);
	}
	
	if (setupInfo.formAInfo.targetInfo.box.contains(loc))
	{
		setupInfo.formAInfo.targetInfo.setHighlighted(true);
	}
	else
	{
		setupInfo.formAInfo.targetInfo.setHighlighted(false);
	}

	if (setupInfo.formBInfo.targetInfo.box.contains(loc))
	{
		setupInfo.formBInfo.targetInfo.setHighlighted(true);
	}
	else
	{
		setupInfo.formBInfo.targetInfo.setHighlighted(false);
	}

	change = true;
}

////////////////////////
///	Battle Functions ///
////////////////////////

void Renderer::setUnitBox(UnitInfoBox* container, Unit* unit, double centerx, double centery, double width, double height)
{
	container->info = unit;
	double xpos = centerx - width / 2;
	double ypos = centery - height / 2;
	container->box = Rect2D(Point2D(xpos, ypos), width, height, WHITE, true);
}

void Renderer::setGroupBox(GroupInfoBox* container, Group* group, Direction dir, double centerx, double centery, double width, double height)
{
	container->info = group;

	double dx = width / (group->getWidth());
	double dy = height / (group->getHeight());
	double unitwidth = dx / 1.20;
	double unitheight = dy / 1.20;
	double startingx = centerx - width / 2;
	double startingy = centery - height / 2;

	container->box = Rect2D(Point2D(startingx, startingy), width, height);
	if (dir == DIRECTION_SOUTH)
		container->box.color = Color(1.0, 0.5, 0.5);
	else if (dir == DIRECTION_NORTH)
		container->box.color = Color(0.5, 0.5, 1.0);
	container->box.filled = true;
	
	container->unitInfos = vector<vector<UnitInfoBox> >(group->getWidth(), vector<UnitInfoBox>(group->getHeight()));
	for (int i = 0; i < group->getWidth(); ++i)
		for (int j = 0; j < group->getHeight(); ++j)
		{
			// can be null
			Unit* unit = group->getUnitAt(i, j);

			int xmod;
			int ymod;
			switch (dir)
			{
				case DIRECTION_NORTH:
					xmod = i;
					ymod = j;
					break;
				case DIRECTION_WEST:
					xmod = j;
					ymod = group->getHeight() - i - 1;
					break;
				case DIRECTION_EAST:
					xmod = group->getWidth() - j - 1;
					ymod = i;
					break;
				case DIRECTION_SOUTH:
					xmod = group->getWidth() - i - 1;
					ymod = group->getHeight() - j - 1;
					break;
				default:
					xmod = i;
					ymod = j;
					break;
			}
			double xpos = startingx + xmod * dx + dx / 2;
			double ypos = startingy + ymod * dy + dx / 2;
			setUnitBox(&container->unitInfos[i][j], unit, xpos, ypos, unitwidth, unitheight);
		}
}

void Renderer::initBattleRenderer(Battle* battle)
{
	battleInfo.info = battle;
	
	double w = (edgeRight - edgeLeft) / 2;
	double h = (edgeBottom - edgeTop);
	battleInfo.sideboardInfo.box = Rect2D(Point2D(0.0, 0.0), w, h, BLACK, true);
	battleInfo.box = Rect2D(Point2D((edgeLeft + edgeRight) / 2, edgeTop), w, h, GRAY, true); 
	
	setGroupBox(&battleInfo.groupAInfo, battle->group1, DIRECTION_NORTH, 0.75, 0.75, 0.40, 0.40);
	setGroupBox(&battleInfo.groupBInfo, battle->group2, DIRECTION_SOUTH, 0.75, 0.25, 0.40, 0.40);
}

void Renderer::renderBattle()
{
	battleInfo.render(this);
}

Renderer::~Renderer()
{
}

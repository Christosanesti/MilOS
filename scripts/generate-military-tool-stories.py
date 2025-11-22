#!/usr/bin/env python3
"""
Script to generate individual story files from military-grade-tools-stories.md
"""

import re
import os
from pathlib import Path

def extract_story_content(md_content, story_num):
    """Extract story content from markdown"""
    # Pattern to match story sections
    pattern = rf'#### Story {story_num}:(.*?)(?=#### Story|\n## |$)'
    match = re.search(pattern, md_content, re.DOTALL)
    if not match:
        return None
    
    story_content = match.group(1).strip()
    return story_content

def parse_story_sections(story_content):
    """Parse story into sections"""
    sections = {}
    
    # Extract Status
    status_match = re.search(r'\*\*Status:\*\*\s*(\w+)', story_content)
    sections['status'] = status_match.group(1) if status_match else 'Draft'
    
    # Extract Epic
    epic_match = re.search(r'\*\*Epic:\*\*\s*(.*?)(?:\n|$)', story_content)
    sections['epic'] = epic_match.group(1).strip() if epic_match else ''
    
    # Extract Priority
    priority_match = re.search(r'\*\*Priority:\*\*\s*(\w+)', story_content)
    sections['priority'] = priority_match.group(1) if priority_match else ''
    
    # Extract Story
    story_match = re.search(r'\*\*Story:\*\*\s*\n\n(.*?)(?=\n\n\*\*Acceptance Criteria:\*\*|$)', story_content, re.DOTALL)
    sections['story'] = story_match.group(1).strip() if story_match else ''
    
    # Extract Acceptance Criteria
    ac_match = re.search(r'\*\*Acceptance Criteria:\*\*\s*\n\n(.*?)(?=\n\n\*\*Tasks / Subtasks:\*\*|$)', story_content, re.DOTALL)
    sections['acceptance_criteria'] = ac_match.group(1).strip() if ac_match else ''
    
    # Extract Tasks
    tasks_match = re.search(r'\*\*Tasks / Subtasks:\*\*\s*\n\n(.*?)(?=\n\n\*\*Dev Notes:\*\*|$)', story_content, re.DOTALL)
    sections['tasks'] = tasks_match.group(1).strip() if tasks_match else ''
    
    # Extract Dev Notes
    dev_notes_match = re.search(r'\*\*Dev Notes:\*\*\s*\n\n(.*?)(?=\n\n---|$)', story_content, re.DOTALL)
    sections['dev_notes'] = dev_notes_match.group(1).strip() if dev_notes_match else ''
    
    return sections

def generate_story_filename(story_num, story_title):
    """Generate story filename"""
    # Clean title for filename
    title_clean = re.sub(r'[^a-zA-Z0-9\s-]', '', story_title)
    title_clean = re.sub(r'\s+', '-', title_clean).lower()
    title_clean = title_clean[:50]  # Limit length
    return f"{story_num}.{title_clean}.story.md"

def create_story_file(story_num, sections, output_dir):
    """Create individual story file"""
    # Extract story title from story content
    story_lines = sections['story'].split('\n')
    title = story_lines[0].strip() if story_lines else f"Story {story_num}"
    
    # Generate filename
    filename = generate_story_filename(story_num, title)
    filepath = Path(output_dir) / filename
    
    # Create story file content
    story_file = f"""# Story {story_num}: {title}

**Status:** {sections['status']}  
**Epic:** {sections['epic']}  
**Created:** $(date)  
**Priority:** {sections['priority']}

---

## Story

{sections['story']}

---

## Acceptance Criteria

{sections['acceptance_criteria']}

---

## Tasks / Subtasks

{sections['tasks']}

---

## Dev Notes

{sections['dev_notes']}

---

## Change Log

| Date | Version | Description | Author |
|------|---------|-------------|--------|
| $(date) | 1.0 | Initial story creation | Scrum Master |

---

## Dev Agent Record

### Agent Model Used

_To be filled by Dev Agent_

### Debug Log References

_To be filled by Dev Agent_

### Completion Notes List

_To be filled by Dev Agent_

### File List

_To be filled by Dev Agent_

---

## QA Results

_To be filled by QA Agent_
"""
    
    # Write file
    with open(filepath, 'w') as f:
        f.write(story_file)
    
    return filepath

def main():
    """Main function"""
    # Read consolidated stories file
    stories_file = Path('docs/stories/military-grade-tools-stories.md')
    if not stories_file.exists():
        print(f"Error: {stories_file} not found")
        return
    
    with open(stories_file, 'r') as f:
        content = f.read()
    
    # Output directory
    output_dir = Path('docs/stories')
    output_dir.mkdir(parents=True, exist_ok=True)
    
    # Find all story numbers
    story_pattern = r'#### Story (\d+\.\d+):'
    story_numbers = re.findall(story_pattern, content)
    
    print(f"Found {len(story_numbers)} stories to process")
    
    created = 0
    for story_num in story_numbers:
        story_content = extract_story_content(content, story_num)
        if not story_content:
            print(f"Warning: Could not extract story {story_num}")
            continue
        
        sections = parse_story_sections(story_content)
        if not sections.get('story'):
            print(f"Warning: Story {story_num} has no content")
            continue
        
        filepath = create_story_file(story_num, sections, output_dir)
        print(f"Created: {filepath}")
        created += 1
    
    print(f"\nCreated {created} story files")

if __name__ == '__main__':
    main()


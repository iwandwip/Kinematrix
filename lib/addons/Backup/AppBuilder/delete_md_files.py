#!/usr/bin/env python3
import os
import argparse
from pathlib import Path

# Default folders to exclude (common development folders)
DEFAULT_EXCLUDE_FOLDERS = {
    'node_modules',      # Node.js dependencies
    '.git',              # Git repository
    '__pycache__',       # Python cache
    '.vscode',           # VS Code settings
    '.idea',             # IntelliJ/PyCharm
    'dist',              # Build output
    'build',             # Build output
    '.next',             # Next.js build
    'coverage',          # Test coverage
    '.pytest_cache',     # Pytest cache
    '.mypy_cache',       # MyPy cache
    'venv',              # Python virtual env
    'env',               # Python virtual env
    '.env',              # Environment files dir
    'vendor',            # Package dependencies
    'target',            # Rust/Java build
    'bin',               # Binary files
    'obj',               # Object files
    '.nuxt',             # Nuxt.js build
    'public',            # Static files (might contain docs)
}

def is_excluded_path(file_path, project_dir, exclude_folders):
    """
    Check if file path contains any excluded folder
    """
    try:
        relative_path = file_path.relative_to(project_dir)
        path_parts = relative_path.parts
        
        # Check if any part of the path is in exclude list
        for part in path_parts:
            if part in exclude_folders:
                return True
        return False
    except ValueError:
        # Path is not relative to project_dir
        return True

def delete_md_files(project_path, dry_run=False, force=False, exclude_folders=None, include_all=False):
    """
    Delete all .md files recursively in the specified directory
    """
    project_dir = Path(project_path).resolve()
    
    if not project_dir.exists():
        print(f"❌ Directory {project_path} does not exist!")
        return []
    
    if not project_dir.is_dir():
        print(f"❌ {project_path} is not a directory!")
        return []
    
    # Determine which folders to exclude
    if include_all:
        final_exclude_folders = set()
    else:
        final_exclude_folders = DEFAULT_EXCLUDE_FOLDERS.copy()
    
    if exclude_folders:
        final_exclude_folders.update(exclude_folders)
    
    # Find all .md files recursively
    all_md_files = list(project_dir.rglob("*.md"))
    
    # Filter out excluded paths
    md_files = []
    excluded_files = []
    
    for md_file in all_md_files:
        if is_excluded_path(md_file, project_dir, final_exclude_folders):
            excluded_files.append(md_file)
        else:
            md_files.append(md_file)
    
    print(f"📁 Scanning: {project_dir}")
    
    if final_exclude_folders:
        print(f"🚫 Excluding folders: {', '.join(sorted(final_exclude_folders))}")
    
    if excluded_files:
        print(f"⏭️  Skipped {len(excluded_files)} .md files in excluded folders")
    
    if not md_files:
        print("✅ No .md files found in the directory (excluding ignored folders).")
        return []
    
    print(f"🔍 Found {len(md_files)} .md files to process:")
    
    for i, md_file in enumerate(md_files, 1):
        relative_path = md_file.relative_to(project_dir)
        print(f"  {i:2d}. {relative_path}")
    
    if dry_run:
        print(f"\n🔍 DRY RUN: Would delete {len(md_files)} .md files")
        if excluded_files:
            print(f"         Would skip {len(excluded_files)} .md files in excluded folders")
        return []
    
    # Confirmation prompt (unless force flag is used)
    if not force:
        print(f"\n⚠️  Are you sure you want to delete all {len(md_files)} .md files?")
        if excluded_files:
            print(f"    ({len(excluded_files)} files in excluded folders will be preserved)")
        response = input("Type 'yes' to confirm: ").strip().lower()
        if response not in ['yes', 'y']:
            print("❌ Operation cancelled.")
            return []
    
    # Proceed with deletion
    deleted_files = []
    deleted_count = 0
    failed_count = 0
    
    print(f"\n🗑️  Deleting files...")
    
    for md_file in md_files:
        try:
            md_file.unlink()
            deleted_files.append(str(md_file))
            deleted_count += 1
            relative_path = md_file.relative_to(project_dir)
            print(f"  ✅ {relative_path}")
        except Exception as e:
            failed_count += 1
            relative_path = md_file.relative_to(project_dir)
            print(f"  ❌ {relative_path}: {e}")
    
    print(f"\n📊 Summary:")
    print(f"  ✅ Successfully deleted: {deleted_count} files")
    if failed_count > 0:
        print(f"  ❌ Failed to delete: {failed_count} files")
    if excluded_files:
        print(f"  ⏭️  Preserved in excluded folders: {len(excluded_files)} files")
    
    return deleted_files

def main():
    parser = argparse.ArgumentParser(
        description="Delete all .md files recursively in a directory (with smart exclusions)",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  %(prog)s                                    # Delete .md files in current directory
  %(prog)s --path /path/to/project            # Delete .md files in specified directory
  %(prog)s --dry-run                          # Preview what would be deleted
  %(prog)s --force                            # Delete without confirmation
  %(prog)s --exclude cache temp               # Add custom exclusions
  %(prog)s --include-all                      # Include normally excluded folders
  %(prog)s --include-all --exclude .git       # Include all except .git

Default excluded folders:
  node_modules, .git, __pycache__, .vscode, .idea, dist, build, .next,
  coverage, .pytest_cache, .mypy_cache, venv, env, .env, vendor, target,
  bin, obj, .nuxt, public
        """
    )
    
    parser.add_argument(
        "--path", 
        type=str, 
        default=".",
        help="Directory path to scan for .md files (default: current directory)"
    )
    
    parser.add_argument(
        "--exclude", 
        nargs="*",
        default=[],
        help="Additional folders to exclude (space-separated)"
    )
    
    parser.add_argument(
        "--include-all", 
        action="store_true",
        help="Include all folders (disable default exclusions)"
    )
    
    parser.add_argument(
        "--dry-run", 
        action="store_true",
        help="Show what files would be deleted without actually deleting them"
    )
    
    parser.add_argument(
        "--force", 
        action="store_true",
        help="Delete files without confirmation prompt"
    )
    
    parser.add_argument(
        "--list-defaults",
        action="store_true",
        help="List default excluded folders and exit"
    )
    
    parser.add_argument(
        "--version",
        action="version",
        version="%(prog)s 1.1.0"
    )
    
    args = parser.parse_args()
    
    # Handle list-defaults
    if args.list_defaults:
        print("Default excluded folders:")
        for folder in sorted(DEFAULT_EXCLUDE_FOLDERS):
            print(f"  - {folder}")
        return
    
    # Print header
    print("🧹 Markdown File Cleaner")
    print("=" * 40)
    
    if args.dry_run:
        print("🔍 DRY RUN MODE - No files will be deleted")
    
    if args.force and not args.dry_run:
        print("⚡ FORCE MODE - No confirmation prompt")
    
    if args.include_all:
        print("🌐 INCLUDE ALL MODE - No default exclusions")
    
    print()
    
    # Execute deletion
    deleted_files = delete_md_files(
        project_path=args.path,
        dry_run=args.dry_run,
        force=args.force,
        exclude_folders=set(args.exclude) if args.exclude else None,
        include_all=args.include_all
    )
    
    if deleted_files and not args.dry_run:
        print(f"\n✨ Operation completed successfully!")

if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print("\n\n❌ Operation cancelled by user (Ctrl+C)")
    except Exception as e:
        print(f"\n❌ Unexpected error: {e}")
        exit(1)
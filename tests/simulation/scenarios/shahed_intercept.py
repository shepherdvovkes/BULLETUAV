#!/usr/bin/env python3
"""
BULLET UAV Shahed Interception Simulation Scenario

This simulation tests the BULLET UAV's ability to intercept
Shahed-type drones under various conditions.
"""

import numpy as np
import matplotlib.pyplot as plt
from dataclasses import dataclass
from typing import List, Tuple

@dataclass
class Target:
    """Shahed drone target model"""
    position: np.ndarray  # [x, y, z] in meters
    velocity: np.ndarray  # [vx, vy, vz] in m/s
    type: str = "shahed_136"
    rcs: float = 0.01     # Radar cross section m²
    
class BulletUAV:
    """BULLET UAV simulation model"""
    
    def __init__(self):
        self.position = np.array([0.0, 0.0, 1000.0])  # Starting position
        self.velocity = np.array([111.0, 0.0, 0.0])   # 400 km/h = 111 m/s
        self.max_speed = 125.0  # 450 km/h max speed
        self.fuel_remaining = 1800  # 30 minutes at full throttle
        
    def update_guidance(self, target: Target, dt: float):
        """Update guidance using proportional navigation"""
        # Simple proportional navigation implementation
        N = 4.0  # Navigation gain
        
        # Calculate line-of-sight vector
        los_vector = target.position - self.position
        los_range = np.linalg.norm(los_vector)
        
        if los_range < 100:  # Terminal guidance phase
            # Switch to impact guidance
            self.velocity = los_vector / los_range * self.max_speed
        else:
            # Proportional navigation
            los_unit = los_vector / los_range
            closing_velocity = np.dot(target.velocity - self.velocity, los_unit)
            
            if closing_velocity > 0:  # Approaching target
                lateral_command = N * closing_velocity * 0.1  # Simplified
                self.velocity += np.array([0, lateral_command, 0]) * dt
                
        # Update position
        self.position += self.velocity * dt
        self.fuel_remaining -= dt
        
    def is_intercept(self, target: Target) -> bool:
        """Check if intercept occurred"""
        distance = np.linalg.norm(target.position - self.position)
        return distance < 10.0  # 10 meter intercept radius

def run_shahed_scenario():
    """Run Shahed interception scenario"""
    
    # Initialize target (Shahed drone)
    shahed = Target(
        position=np.array([50000.0, 10000.0, 500.0]),  # 50km away
        velocity=np.array([-51.4, 0.0, 0.0])  # 185 km/h = 51.4 m/s
    )
    
    # Initialize BULLET UAV
    bullet = BulletUAV()
    
    # Simulation parameters
    dt = 0.1  # 100ms time step
    max_time = 600  # 10 minutes max
    
    # Data logging
    times = []
    bullet_positions = []
    target_positions = []
    ranges = []
    
    print("Starting Shahed interception scenario...")
    print(f"Initial range: {np.linalg.norm(shahed.position - bullet.position):.0f}m")
    
    # Run simulation
    time = 0.0
    while time < max_time and bullet.fuel_remaining > 0:
        # Update target position
        shahed.position += shahed.velocity * dt
        
        # Update BULLET guidance and position
        bullet.update_guidance(shahed, dt)
        
        # Check for intercept
        if bullet.is_intercept(shahed):
            print(f"INTERCEPT at t={time:.1f}s")
            print(f"Range at intercept: {np.linalg.norm(shahed.position - bullet.position):.1f}m")
            break
            
        # Log data
        times.append(time)
        bullet_positions.append(bullet.position.copy())
        target_positions.append(shahed.position.copy())
        ranges.append(np.linalg.norm(shahed.position - bullet.position))
        
        time += dt
    
    # Plot results
    plt.figure(figsize=(12, 8))
    
    # Trajectory plot
    plt.subplot(2, 2, 1)
    bullet_pos = np.array(bullet_positions)
    target_pos = np.array(target_positions)
    
    plt.plot(bullet_pos[:, 0]/1000, bullet_pos[:, 1]/1000, 'b-', label='BULLET UAV')
    plt.plot(target_pos[:, 0]/1000, target_pos[:, 1]/1000, 'r-', label='Shahed')
    plt.xlabel('X Position (km)')
    plt.ylabel('Y Position (km)')
    plt.title('Interception Trajectory')
    plt.legend()
    plt.grid(True)
    
    # Range vs time
    plt.subplot(2, 2, 2)
    plt.plot(times, np.array(ranges)/1000, 'g-')
    plt.xlabel('Time (s)')
    plt.ylabel('Range (km)')
    plt.title('Range to Target')
    plt.grid(True)
    
    # Altitude profile
    plt.subplot(2, 2, 3)
    plt.plot(times, bullet_pos[:, 2], 'b-', label='BULLET')
    plt.plot(times, target_pos[:, 2], 'r-', label='Shahed')
    plt.xlabel('Time (s)')
    plt.ylabel('Altitude (m)')
    plt.title('Altitude Profile')
    plt.legend()
    plt.grid(True)
    
    # Speed profile
    plt.subplot(2, 2, 4)
    bullet_speeds = [np.linalg.norm(bullet.velocity) * 3.6 for _ in times]  # km/h
    plt.plot(times, bullet_speeds, 'b-')
    plt.xlabel('Time (s)')
    plt.ylabel('Speed (km/h)')
    plt.title('BULLET Speed')
    plt.grid(True)
    
    plt.tight_layout()
    plt.savefig('shahed_intercept_results.png')
    plt.show()
    
    return time < max_time

if __name__ == "__main__":
    success = run_shahed_scenario()
    if success:
        print("✓ Shahed interception successful")
    else:
        print("✗ Shahed interception failed")

